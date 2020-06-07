open Belt;
open ReactUtil;
open SetUtil;

type appComponent =
  | CodeEditor
  | Help
  | CanvasExperiment;

type appTopLevelCommand =
  | Login(string)
  | ToggleAppComponent(appComponent)
  | ToggleDebugMode(DebugMode.debugMode);

// About Set: https://stackoverflow.com/a/58268653/1148030


module AppComponentComparable =
  Id.MakeComparable({
    type t = appComponent;
    let cmp = Pervasives.compare;
  });

type appComponents = Set.t(appComponent, AppComponentComparable.identity);

type appMainView = 
  | Login
  | Coding(appComponents)

type appTopLevelState = {
  appMainView,
  debugModes: DebugMode.debugModes,
};

let initialComponents = Set.fromArray([|CanvasExperiment, CodeEditor, Help|], ~id=(module AppComponentComparable));

let initial: appTopLevelState = {appMainView: Login, debugModes: DebugMode.initial};

let appTopLevelStateReducer = (prev: appTopLevelState, command: appTopLevelCommand): appTopLevelState => {
  switch (command) {
    | Login(loginName) =>  
      CodeCanvasState.dispatch(CodeCanvasState.Login(loginName));
      {...prev, appMainView: Coding(initialComponents)}
    | ToggleAppComponent(component) => 
    switch (prev.appMainView) {
      | Login => prev
      | Coding(appComponents) => {...prev, appMainView: Coding(setToggle(appComponents, component))}
    }
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

  let elements: list(reactComponent) =
    switch (state.appMainView) {
      | Login => [<Login key="login" loginFunction={(loginName) => dispatchCommand(Login(loginName))}/>]
      | Coding(appComponents) => [
        if (Set.has(appComponents, CanvasExperiment)) {
          <MainCanvas key="mainCanvas"/>;
        } else {
          emptyFragment;
        },    
        if (Set.has(appComponents, CodeEditor)) {
          <CodeEditor key="codeEditor"/>;
        } else {
          emptyFragment;
        },  
        if (Set.has(appComponents, Help)) {
          <>
          <HelpCell key="help1"/>
          <HelpCell2 key="help2"/>
          <HelpCell3 key="help3"/>
          </>
        } else {
          emptyFragment;
          }
        ]
    };

  React.useEffect0(debugKeyboardListenerEffect(dispatchCommand));

  <> 
  <div className="topLevelGrid">
  <div className="titleCell"><h1>{React.string("Koodipaja")}</h1></div>
  {asReact(elements)}
  <div className="canvasProbeCell">
  <div id="canvasLayoutProbe" className="canvasProbeDiv"/>
  </div>
  </div></>;
};