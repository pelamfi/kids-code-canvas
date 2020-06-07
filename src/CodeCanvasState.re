open Note;
open SynthState;
open AnimationConstants;

type scriptlet = {
  scriptlet: string,  
  evalFunction: Scriptlet.scriptletFunction,
  loginName: string,
}

let initialScriptlet = {scriptlet: "", evalFunction: Scriptlet.initial, loginName: ""}

type stateChange =
  | Voice(voice)
  | RenderFramesTo(int)
  | ScriptletFunctionChanged(scriptlet)
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
  scriptlet: scriptlet,
  listeners: list(stateChange => unit),
  lastUpdate: list(stateChange),
};

let expectedAnimFrameCount = (times: frameTimes): int => int_of_float((times.lastMs -. times.firstMs) /. targetFrameIntervalMs)

let initialState: state = {
  updateIndex: 0,
  frameTimes: None,
  expectedFrameCount: 0,
  scriptlet: initialScriptlet,
  listeners: [],
  lastUpdate: [],
};

type event =
  | Login(string)
  | ChangeScriptlet(string)
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
    | Login(loginName) => {
      {...initialState, scriptlet: {...initialScriptlet, loginName}}
    }
    | ChangeScriptlet(scriptletString) => {
      let evalFunction = Scriptlet.compileScriptlet(scriptletString);
      let expectedFrameCount = 0;
      let scriptlet = {...state.scriptlet, scriptlet: scriptletString, evalFunction};
      {...state, 
        scriptlet, 
        frameTimes: None, 
        expectedFrameCount, 
        lastUpdate: [RenderFramesTo(expectedFrameCount), ScriptletFunctionChanged(scriptlet)]}
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