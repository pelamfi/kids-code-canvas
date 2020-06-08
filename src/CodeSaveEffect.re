
type effectCleanup = unit => unit;

type effect = unit => option(effectCleanup);

open Fetch;

let save = (scriptlet: CodeCanvasState.scriptlet): unit => {
  if (scriptlet.loginName != "" && scriptlet.workshopId != "") {
    let path = Printf.sprintf("/api/0/%s/%s", scriptlet.workshopId, scriptlet.loginName);
    Js.log(path);
    Fetch.fetchWithInit(
        path,
        Fetch.RequestInit.make(
          ~method_=Post,
          ~body=Fetch.BodyInit.make(scriptlet.scriptletString),
          ~headers=Fetch.HeadersInit.make({"Content-Type": "text/text"}),
          ()
        )
      ) |> ignore;
  }
};

let codeSaveEffect = (isCoding: bool): effect => {
  let debounceSave = Throttle.debounce(save);  
  () => CodeCanvasState.listenerEffect(stateChange =>
    switch (stateChange) {
    | ScriptletFunctionChanged(compiledScriptlet) when isCoding =>
      debounceSave(compiledScriptlet.scriptlet);
      ()
    | _ => ()
    }, CodeCanvasState.dispatch, ()
  );
};