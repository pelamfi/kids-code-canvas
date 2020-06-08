
type effectCleanup = unit => unit;

type effect = unit => option(effectCleanup);

open Fetch;
open Js.Promise;

let loginEffect = (login: option((string, string)), dispatch: string => unit): effect => {
  () => 
  switch(login) {
    | Some((workshopId, loginName)) =>
      let cancelableDispatch: ref(option(string => unit)) = ref(Some(dispatch));
      let path = Printf.sprintf("/api/0/%s/%s", workshopId, loginName);
      Fetch.fetchWithInit(
        path,
        Fetch.RequestInit.make(
          ~method_=Get,
          ()
        )
      )
       |> then_(Fetch.Response.text)
       |> then_(text => cancelableDispatch^ -> Belt.Option.mapWithDefault((), d => d(text)) |> resolve)
       |> ignore;
      Some(() => cancelableDispatch := None)
    | None => None
  }
};