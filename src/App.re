open Belt;
open ReactUtil;
open SetUtil;

type appComponent =
  | CodeEditor
  | Help
  | CanvasExperiment;

type appTopLevelCommand =
  | Login(string)
  | Url(ReasonReactRouter.url)
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
  urlPath: string,
  debugModes: DebugMode.debugModes,
};

let initialComponents = Set.fromArray([|CanvasExperiment, CodeEditor, Help|], ~id=(module AppComponentComparable));

// Clear hash and query, since we are not using them now
let urlPath = (urlPath: list(string)): string => 
  Js.Array.joinWith("/", Belt.List.toArray(urlPath));

let initial: appTopLevelState = {appMainView: Login, urlPath: urlPath([]), debugModes: DebugMode.initial};

let appTopLevelStateReducer = (prev: appTopLevelState, command: appTopLevelCommand): appTopLevelState => {
  switch (command) {
    | Url(url) => 
      switch(url.path) {
        | ["workshop", "eb039e58-748a-406e-a6cb-cdfdf660d866"] | ["workshoptest"]=> // workshop mode, ask user name
          {...prev, urlPath: urlPath(["workshop", "eb039e58-748a-406e-a6cb-cdfdf660d866"]), appMainView: Login}
        | ["workshop", "eb039e58-748a-406e-a6cb-cdfdf660d866", user] | [user] => // workshop mode with user name
          CodeCanvasState.dispatch(CodeCanvasState.Login(user));
          {...prev, urlPath: urlPath(["workshop", "eb039e58-748a-406e-a6cb-cdfdf660d866", user]), appMainView: Coding(initialComponents)}
        | _ =>
          {...prev, urlPath: urlPath([]), appMainView: Coding(initialComponents)} // no backend, non workshop mode
      }
    | Login(loginName) =>  
      CodeCanvasState.dispatch(CodeCanvasState.Login(loginName));
      {...prev, urlPath: urlPath(["workshop", "eb039e58-748a-406e-a6cb-cdfdf660d866", loginName]), appMainView: Coding(initialComponents)}
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


[@react.component]
let make = () => {
  let (state, dispatchCommand) = React.useReducer(appTopLevelStateReducer, initial);

  let url = ReasonReactRouter.useUrl();

  React.useEffect2(() => dispatchCommand(Url(url)) |> () => None, ((), url));

  React.useEffect2(() => ReasonReactRouter.replace(state.urlPath) |> () => None, ((), state.urlPath));

  React.useEffect2(TimerUpdateEffect.timerUpdateEffect(true, CodeCanvasState.dispatch), ((), true));

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

  <div className="topLevelGrid">
  
  <div className="titleCell"><h1>{React.string("Koodipaja")}</h1></div>

  {asReact(elements)}

  <div className="canvasProbeCell">
  <div id="canvasLayoutProbe" className="canvasProbeDiv"/>
  </div>

  </div>
};