// https://stackoverflow.com/a/58689472/1148030
open CanvasUtil

type canvasState = {frame: int};

let initialCanvasState = {frame: 0}

let drawOnCanvas =
    (canvasState, context: canvasDrawContext): unit => {
  open Webapi.Canvas.Canvas2d;
  clearRect(context.context, ~x=0., ~y=0., ~w=context.dimensions.width, ~h=context.dimensions.height);
  setFillStyle(context.context, String, "red");
  fillRect(context.context, ~x=float(canvasState.frame), ~y=0., ~w=context.dimensions.width, ~h=context.dimensions.height);
};

let frameChangeListenerEffect =
    (setCurrentFrame: int => unit): ((CodeCanvasState.acceptEvent, unit) => option(unit => unit)) => {
  CodeCanvasState.listenerEffect(stateChange =>
    switch (stateChange) {
    | FrameChanged(frame) => setCurrentFrame(frame)
    | _ => ()
    }
  );
};


[@react.component]
let make = () => {
  open React;
  let canvasElementRef: Ref.t(option(Dom.element)) = useRef(None);
  let (currentFrame, setCurrentFrame) = React.useReducer((_, x) => x, 0);

  useLayoutEffect2(() => {
    Ref.current(canvasElementRef)
    |> Belt.Option.map(_, drawOnCanvasElement(drawOnCanvas({frame: currentFrame})))
    |> ignore;
    None;
  }, (false, currentFrame));

  React.useEffect0(frameChangeListenerEffect(setCurrentFrame, CodeCanvasState.dispatch));  

  <canvas
    width="1920"
    height="960"
    ref={ReactDOMRe.Ref.callbackDomRef(elem =>
      React.Ref.setCurrent(canvasElementRef, Js.Nullable.toOption(elem))
    )}
  />;
};