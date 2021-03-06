open Belt;
open ReactUtil;
open SetUtil;

type appComponent =
  | CodeEditor
  | Help
  | CanvasExperiment;

type appTopLevelCommand =
  | Login(string)
  | LoginSuccess(CodeCanvasState.compiledScriptlet)
  | Url(ReasonReactRouter.url)
  | ToggleAppComponent(appComponent)
  | ToggleDebugMode(DebugMode.debugMode);

module AppComponentComparable =
  Id.MakeComparable({
    type t = appComponent;
    let cmp = Pervasives.compare;
  });

// About Set: https://stackoverflow.com/a/58268653/1148030
type appComponents = Set.t(appComponent, AppComponentComparable.identity);

type appMainState = 
  | Login(string)
  | LoggingIn(string, string)
  | Coding(appComponents)

type appTopLevelState = {
  appMainState,
  urlPath: list(string),
  debugModes: DebugMode.debugModes,
  compiledScriptlet: CodeCanvasState.compiledScriptlet
};

let initialComponents = Set.fromArray([|CanvasExperiment, CodeEditor, Help|], ~id=(module AppComponentComparable));

// Clear hash and query, since we are not using them now
let urlPath = (urlPath: list(string)): string => 
  "/" ++ Js.Array.joinWith("/", Belt.List.toArray(urlPath));

let initial: appTopLevelState = {
  appMainState: Coding(initialComponents), 
  urlPath: [],
  debugModes: DebugMode.initial, 
  compiledScriptlet: CodeCanvasState.initialScriptlet
};

let appTopLevelStateReducer = (prev: appTopLevelState, command: appTopLevelCommand): appTopLevelState => {
  switch (prev.appMainState, command) {
    | (_, Url(url)) => 
      switch(url.path) {
        | [test] when test == "logintest" => // test login
          {...prev, compiledScriptlet: CodeCanvasState.initialScriptlet, appMainState: LoggingIn("eb039e58-748a-406e-a6cb-cdfdf660d866", "logintest")}
        | [test] when test == "workshoptest" => // test tworkshop login page
          {...prev, compiledScriptlet: CodeCanvasState.initialScriptlet, appMainState: Login("eb039e58-748a-406e-a6cb-cdfdf660d866")}
        | ["workshop", workshopId] => // workshop mode, ask user name
          {...prev, compiledScriptlet: CodeCanvasState.initialScriptlet, appMainState: Login(workshopId)}
        | ["workshop", workshopId, user] => // workshop mode with user name
          if ((prev.compiledScriptlet.scriptlet.workshopId, prev.compiledScriptlet.scriptlet.loginName) == (workshopId, user)) {
            prev // logged in user info matches
          } else {
            // different user, or not logged in. Starting to login
            {...prev, appMainState: LoggingIn(workshopId, user)} 
          }
        | _ =>
          // no backend, non workshop mode
          {...prev, compiledScriptlet: CodeCanvasState.initialScriptlet, urlPath: [], appMainState: Coding(initialComponents)} 
      }
    | (Login(workshopId), Login(loginName)) =>  
      {...prev, appMainState: LoggingIn(workshopId, loginName)}
    | (LoggingIn(workshopId, loginName), LoginSuccess(compiledScriptlet)) =>
      {...prev, compiledScriptlet, urlPath: ["workshop", workshopId, loginName], appMainState: Coding(initialComponents)}
    | (_, ToggleAppComponent(component)) => 
      switch (prev.appMainState) {
        | Coding(appComponents) => {...prev, appMainState: Coding(setToggle(appComponents, component))}
        | _ => prev
      }
    | (_, ToggleDebugMode(mode)) => {...prev, debugModes: setToggle(prev.debugModes, mode)}
    | _ => prev
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


let isCoding = (appMainState: appMainState): bool => switch (appMainState) {
    | Coding(_) => true
    | _ => false
};

let getLoggingIn = (appMainState: appMainState): option((string, string)) => switch (appMainState) {
    | LoggingIn(workshopId, loginName) => Some((workshopId, loginName))
    | _ => None
};

let mainAppStateChangeListenerEffect =
    (dispatch: dispatch): ((CodeCanvasState.acceptEvent, unit) => option(unit => unit)) => {
  CodeCanvasState.listenerEffect(stateChange =>
    switch (stateChange) {
    | LoggedIn(compiledScriptlet) => 
      dispatch(LoginSuccess(compiledScriptlet))
    | _ => ()
    }
  );
};

let urlUpdateEffect =
  (currentUrl: ReasonReactRouter.url, wantedPath: list(string)): (unit => option(unit => unit)) => {
  () => {
    if (currentUrl.path != wantedPath && List.length(wantedPath) > 0) {
      let path = urlPath(wantedPath);
      Js.log("hisory push " ++ path)  
      ReasonReactRouter.push(path)
    }
    None
  }
};

[@react.component]
let make = () => {
  let (state, dispatchCommand) = React.useReducer(appTopLevelStateReducer, initial);

  let url = ReasonReactRouter.useUrl();

  React.useEffect2(() => dispatchCommand(Url(url)) |> () => None, ((), url));

  React.useEffect2(urlUpdateEffect(url, state.urlPath), ((), state.urlPath));

  let isCoding = isCoding(state.appMainState);  

  React.useEffect2(CodeSaveEffect.codeSaveEffect(isCoding), ((), isCoding));

  React.useEffect2(TimerUpdateEffect.timerUpdateEffect(isCoding, CodeCanvasState.dispatch), ((), isCoding));

  React.useEffect0(mainAppStateChangeListenerEffect(dispatchCommand, CodeCanvasState.dispatch));

  let loggingIn = getLoggingIn(state.appMainState)

  React.useEffect2(LoginEffect.loginEffect(loggingIn), ((), loggingIn));

  let elements: list(reactComponent) =
    switch (state.appMainState) {
      | Login(_) => [<Login key="login" loginFunction={(loginName) => dispatchCommand(Login(loginName))}/>]
      | LoggingIn(_, loginName) => [
        <div key="loggingIn" className="loggingIn">{ReasonReact.string("Kirjaudutaan sisään " ++ loginName ++". Odota hetki...")}
        </div>
        ]
      | Coding(appComponents) => [
        if (Set.has(appComponents, CanvasExperiment)) {
          <MainCanvas key="mainCanvas" compiledScriptlet=state.compiledScriptlet/>;
        } else {
          emptyFragment;
        },    
        if (Set.has(appComponents, CodeEditor)) {
          <React.Fragment key = "codeEditorCells">  
          <CodeEditor key="codeEditor" compiledScriptlet=state.compiledScriptlet/>
          <Controls key="controls"/>
          </React.Fragment>
        } else {
          emptyFragment;
        },  
        if (Set.has(appComponents, Help)) {
          <HelpCells key="helpCells"/>
        } else {
          emptyFragment;
        }
        ]
    };

  React.useEffect0(debugKeyboardListenerEffect(dispatchCommand));

  <div key="topLevelGrid" className="topLevelGrid">
  
  <div key="titleCell" className="titleCell"><h1>{React.string("Koodipaja")}</h1></div>

  {asReact(elements)}

  <div key="canvasProbeCell" className="canvasProbeCell">
  <div key="canvasLayoutProbe" id="canvasLayoutProbe" className="canvasLayoutProbe"/>
  </div>

  </div>
};