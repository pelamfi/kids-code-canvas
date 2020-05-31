open Belt;
open ReactUtil;
open SetUtil;

type appComponent =
  | NoteInfoStrips
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

let initialComponents = Set.fromArray([|CanvasExperiment|], ~id=(module AppComponentComparable));

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
        | "KeyS" => dispatch(ToggleAppComponent(NoteInfoStrips))
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
      <CanvasExperiment key="canvasExperiment"/>;
    } else {
      emptyFragment;
    },
    if (Set.has(state.appComponents, NoteInfoStrips)) {
      <p>{ReasonReact.string("foo")}</p>;
    } else {
      emptyFragment;
    },
  ];

  React.useEffect0(debugKeyboardListenerEffect(dispatchCommand));

  <> {asReact(elements)} </>;
};