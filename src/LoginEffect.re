
open Js.Promise;

type effectCleanup = unit => unit;

type effect = unit => option(effectCleanup);

let processResponse = (workshopId: string, loginName: string, scriptletString: string): Js.Promise.t(CodeCanvasState.scriptlet) => {
  let scriptlet: CodeCanvasState.scriptlet = {scriptletString, workshopId, loginName};
  Js.Promise.resolve(scriptlet)
}

let loginEffect = (login: option((string, string))): effect => {
  let cancelableDispatch = ref(Some(scriptlet => CodeCanvasState.dispatch(CodeCanvasState.Login(scriptlet))));
  () => 
  switch(login) {
    | Some((workshopId, loginName)) =>
      let path = Printf.sprintf("/api/0/%s/%s", workshopId, loginName);
      Js.log("login request " ++ path);
      Fetch.fetch(path)
       |> then_(Fetch.Response.text)
       |> then_(processResponse(workshopId, loginName))
       |> then_(text => cancelableDispatch^ -> Belt.Option.mapWithDefault((), d => d(text)) |> resolve)
       |> ignore;
      Some(() => cancelableDispatch := None)
    | None => None
  }
};