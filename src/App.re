open Belt;
open ReactUtil;
open SetUtil;

type appComponent =
  | CodeEditor
  | Help
  | CanvasExperiment;

type appTopLevelCommand =
  | ToggleAppComponent(appComponent)
  | ToggleDebugMode(DebugMode.debugMode);

// About Set: https://stackoverflow.com/a/58268653/1148030

module AppComponentComparable =
  Id.MakeComparable({
    type t = appComponent;
    let cmp = Pervasives.compare;
  });

type appComponents = Set.t(appComponent, AppComponentComparable.identity);

type appTopLevelState = {
  appComponents,
  debugModes: DebugMode.debugModes,
};

let initialComponents = Set.fromArray([|CanvasExperiment, CodeEditor, Help|], ~id=(module AppComponentComparable));

let initial: appTopLevelState = {appComponents: initialComponents, debugModes: DebugMode.initial};

let appTopLevelStateReducer = (prev: appTopLevelState, command: appTopLevelCommand): appTopLevelState => {
  switch (command) {
  | ToggleAppComponent(component) => {...prev, appComponents: setToggle(prev.appComponents, component)}
  | ToggleDebugMode(mode) => {...prev, debugModes: setToggle(prev.debugModes, mode)}
  };
};

type dispatch = appTopLevelCommand => unit;

let debugKeyboardListenerEffect = (dispatch: dispatch, _): option(unit => unit) => {
  open Webapi.Dom;

  let keyUpListener = (event: Dom.keyboardEvent): unit => {
    KeyboardEvent.(
      if (shiftKey(event) && ctrlKey(event) && altKey(event)) {
        let code = code(event);
        switch (code) {
        | "KeyC" => dispatch(ToggleAppComponent(CanvasExperiment))
        | "KeyE" => dispatch(ToggleAppComponent(CodeEditor))
        | "KeyH" => dispatch(ToggleAppComponent(Help))
        | "KeyZ" => dispatch(ToggleDebugMode(NoteInfoStrips2xZoom))
        | _ => ()
        };
      }
    );
  };

  let eventTarget = Document.asEventTarget(document);
  EventTarget.addKeyUpEventListener(keyUpListener, eventTarget);
  Some(() => EventTarget.removeKeyUpEventListener(keyUpListener, eventTarget));
};

type effectCleanup = unit => unit;

type effect = unit => option(effectCleanup);

let timerUpdateEffect = (isAnimating: bool, dispatch: (CodeCanvasState.event) => unit): effect => {
  () =>
    if (isAnimating) {
      Some(GroupedRaf.register((timerMs: float) => dispatch(AnimationFrame(timerMs))));
    } else {
      None;
    };
};

[@react.component]
let make = () => {
  let (state, dispatchCommand) = React.useReducer(appTopLevelStateReducer, initial);
  
  React.useEffect2(timerUpdateEffect(true, CodeCanvasState.dispatch), ((), true));

  let elements: list(reactComponent) = [
    if (Set.has(state.appComponents, CanvasExperiment)) {
      <MainCanvas key="mainCanvas"/>;
    } else {
      emptyFragment;
    },    
    if (Set.has(state.appComponents, CodeEditor)) {
      <CodeEditor key="codeEditor"/>;
    } else {
      emptyFragment;
    },  
    if (Set.has(state.appComponents, Help)) {
      <HelpCell key="help1"/>;
    } else {
      emptyFragment;
    },
    if (Set.has(state.appComponents, Help)) {
      <HelpCell2 key="help2"/>;
    } else {
      emptyFragment;
    },  
    if (Set.has(state.appComponents, Help)) {
      <HelpCell3 key="help3"/>;
    } else {
      emptyFragment;
    },  
  ];

  React.useEffect0(debugKeyboardListenerEffect(dispatchCommand));

  <> 
  <div className="topLevelGrid">{asReact(elements)}
  <div className="canvasProbeCell">
  <div id="canvasLayoutProbe" className="canvasProbeDiv"/>
  </div>
  </div></>;
};