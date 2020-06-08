open Belt;
open ReactUtil;
open SetUtil;

type appComponent =
  | CodeEditor
  | Help
  | CanvasExperiment;

type appTopLevelCommand =
  | Login(string)
  | LoginSuccess(string)
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
  urlPath: string,
  debugModes: DebugMode.debugModes,
};

let initialComponents = Set.fromArray([|CanvasExperiment, CodeEditor, Help|], ~id=(module AppComponentComparable));

// Clear hash and query, since we are not using them now
let urlPath = (urlPath: list(string)): string => 
  Js.Array.joinWith("/", Belt.List.toArray(urlPath));

let initial: appTopLevelState = {appMainState: Coding(initialComponents), urlPath: urlPath([]), debugModes: DebugMode.initial};

let appTopLevelStateReducer = (prev: appTopLevelState, command: appTopLevelCommand): appTopLevelState => {
  switch (prev.appMainState, command) {
    | (_, Url(url)) => 
      switch(url.path) {
        | [test] when test == "logintest" => // test login
          {...prev, appMainState: LoggingIn("eb039e58-748a-406e-a6cb-cdfdf660d866", "logintest")}
        | [test] when test == "workshoptest" => // test tworkshop login page
          {...prev, appMainState: Login("eb039e58-748a-406e-a6cb-cdfdf660d866")}
        | ["workshop", workshopId] => // workshop mode, ask user name
          {...prev, appMainState: Login(workshopId)}
        | ["workshop", workshopId, user] => // workshop mode with user name
          {...prev, appMainState: LoggingIn(workshopId, user)}
        | _ =>
          {...prev, urlPath: urlPath([]), appMainState: Coding(initialComponents)} // no backend, non workshop mode
      }
    | (Login(workshopId), Login(loginName)) =>  
      {...prev, urlPath: urlPath(["workshop", workshopId, loginName]), appMainState: LoggingIn(workshopId, loginName)}
    | (LoggingIn(workshopId, loginName), LoginSuccess(scriptletString)) =>
      CodeCanvasState.dispatch(CodeCanvasState.Login({workshopId, loginName, scriptletString}));
      {...prev, urlPath: urlPath(["workshop", workshopId, loginName]), appMainState: Coding(initialComponents)}
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

[@react.component]
let make = () => {
  let (state, dispatchCommand) = React.useReducer(appTopLevelStateReducer, initial);

  let url = ReasonReactRouter.useUrl();

  React.useEffect2(() => dispatchCommand(Url(url)) |> () => None, ((), url));

  React.useEffect2(() => ReasonReactRouter.replace(state.urlPath) |> () => None, ((), state.urlPath));

  let isCoding = isCoding(state.appMainState);  

  React.useEffect2(CodeSaveEffect.codeSaveEffect(isCoding), ((), isCoding));

  React.useEffect2(TimerUpdateEffect.timerUpdateEffect(isCoding, CodeCanvasState.dispatch), ((), isCoding));

  let loggingIn = getLoggingIn(state.appMainState)

  React.useEffect2(LoginEffect.loginEffect(loggingIn, text => dispatchCommand(LoginSuccess(text))), ((), loggingIn));

  let elements: list(reactComponent) =
    switch (state.appMainState) {
      | Login(_) => [<Login key="login" loginFunction={(loginName) => dispatchCommand(Login(loginName))}/>]
      | LoggingIn(_, loginName) => [
        <div className="loggingIn">{ReasonReact.string("Kirjaudutaan sisään " ++ loginName ++". Odota hetki...")}
        </div>
        ]
      | Coding(appComponents) => [
        if (Set.has(appComponents, CanvasExperiment)) {
          <MainCanvas key="mainCanvas"/>;
        } else {
          emptyFragment;
        },    
        if (Set.has(appComponents, CodeEditor)) {
          <CodeEditor key="codeEditor"/>
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