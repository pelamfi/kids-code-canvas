// https://stackoverflow.com/a/58689472/1148030
open CanvasUtil

let drawOnCanvas =
    (context: canvasDrawContext): unit => {
  open Webapi.Canvas.Canvas2d;
  clearRect(context.context, ~x=0., ~y=0., ~w=context.dimensions.width, ~h=context.dimensions.height);
};

[@react.component]
let make = () => {
  open React;
  let canvasElementRef: Ref.t(option(Dom.element)) = useRef(None);

  useLayoutEffect0(() => {
    Ref.current(canvasElementRef)
    |> Belt.Option.map(_, drawOnCanvasElement(drawOnCanvas))
    |> ignore;
    None;
  });

  <canvas
    width="35"
    height="60"
    ref={ReactDOMRe.Ref.callbackDomRef(elem =>
      React.Ref.setCurrent(canvasElementRef, Js.Nullable.toOption(elem))
    )}
  />;
};