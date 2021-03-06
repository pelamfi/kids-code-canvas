open Note;
open SynthState;
open AnimationConstants;

type scriptlet = {
  scriptletString: string,  
  workshopId: string,
  loginName: string,
}

type compiledScriptlet = {
  scriptlet: scriptlet,
  evalFunction: Scriptlet.scriptletFunction
}

let compile = (scriptlet: scriptlet): compiledScriptlet => {
  let evalFunction = Scriptlet.compileScriptlet(scriptlet.scriptletString);
  {scriptlet, evalFunction}
}

let initialScriptlet = compile({scriptletString: "", workshopId: "", loginName: ""})

type stateChange =
  | Voice(voice)
  | RenderFramesTo(int)
  | ScriptletFunctionChanged(compiledScriptlet)
  | LoggedIn(compiledScriptlet)
  | CurrentNoteChanged(note);

type listener = stateChange => unit;

type frameTimes = {
  firstMs: float,
  lastMs: float
}

type state = {
  updateIndex: int,
  frameTimes: option(frameTimes),
  expectedFrameCount: int,
  compiledScriptlet: compiledScriptlet,
  listeners: list(stateChange => unit),
  lastUpdate: list(stateChange),
};

let expectedAnimFrameCount = (times: frameTimes): int => int_of_float((times.lastMs -. times.firstMs) /. targetFrameIntervalMs)

let initialState: state = {
  updateIndex: 0,
  frameTimes: None,
  expectedFrameCount: 0,
  compiledScriptlet: initialScriptlet,
  listeners: [],
  lastUpdate: [],
};

type event =
  | Login(scriptlet)
  | ChangeScriptlet(string)
  | RestartAnimation
  | AnimationFrame(float)
  | RegisterListener(listener)
  | UnregisterListener(listener);

type acceptEvent = event => unit;

let emit = (state: state, stateChange: stateChange) => {
  Belt.List.forEach(state.listeners, listener => listener(stateChange));
};

let updateState = (prevState: state, event: event): state => {
  let state = {...prevState, updateIndex: prevState.updateIndex + 1, lastUpdate: []};

  let newState: state =
    switch (event) {
    | Login(scriptlet) => {
      let compiledScriptlet = compile(scriptlet);
      {...initialState, compiledScriptlet, lastUpdate: [LoggedIn(compiledScriptlet), ScriptletFunctionChanged(compiledScriptlet)]}
    }
    | ChangeScriptlet(scriptletString) => {
      let scriptlet = {...state.compiledScriptlet.scriptlet, scriptletString};
      let compiledScriptlet = compile(scriptlet)
      let expectedFrameCount = 0;
      {...state, 
        compiledScriptlet, 
        frameTimes: None, 
        expectedFrameCount, 
        lastUpdate: [RenderFramesTo(expectedFrameCount), ScriptletFunctionChanged(compiledScriptlet)]}
    }
    | RestartAnimation => {
      let expectedFrameCount = 0;
      {...state, 
        frameTimes: None, 
        expectedFrameCount, 
        lastUpdate: [RenderFramesTo(expectedFrameCount)]}
    }
    | AnimationFrame(frameMs) => {
      let firstMs: float = Belt.Option.map(state.frameTimes, t => t.firstMs) |> OptionUtil.getWithDefaultF(() => frameMs);  
      let frameTimes: frameTimes = {firstMs: firstMs, lastMs: frameMs};
      let expectedFrameCount = expectedAnimFrameCount(frameTimes);
      {...state, expectedFrameCount, frameTimes: Some(frameTimes), lastUpdate:[RenderFramesTo(expectedFrameCount)]}
    }
    | RegisterListener(listener) => {...state, listeners: [listener, ...state.listeners]}
    | UnregisterListener(listener) => {
        ...state,
        listeners: Belt.List.keep(state.listeners, registered => registered !== listener),
      }
    };

  Belt.List.forEach(newState.lastUpdate, emit(state));

  // Js.log(Printf.sprintf("RelativeNotesState transition to %s on %s", stringOfState(newState), stringOfEvent(event)));

  newState;
};

let stateRef: ref(state) = ref(initialState);

let dispatch = (event: event): unit => {
  stateRef := updateState(stateRef^, event);
};

// To help wrapping in react effects
let listenerEffect = (listener: stateChange => unit, dispatch: acceptEvent, _): option(unit => unit) => {
  dispatch(RegisterListener(listener));
  Some(() => dispatch(UnregisterListener(listener)));
};