
open CodeCanvasState;

let reloadHandler: (ReactEvent.Mouse.t) => unit = [%bs.raw
  {|
function reloadHandlerF(event) {
    console.log("foo");
  location.reload();
}
|}];

let restartHandler = (_: ReactEvent.Mouse.t): unit => {
    CodeCanvasState.dispatch(RestartAnimation);
};

[@react.component]
let make = () => {
  <div className="controlsWrapper">
    <a className="controlButton" onClick=restartHandler>
      {React.string("Animaation alkuun")}
    </a>    
    <a className="controlButton" onClick=reloadHandler>
      {React.string("Lataa sivu uudelleen")}
    </a>
  </div>
}
