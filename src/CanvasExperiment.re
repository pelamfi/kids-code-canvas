// https://stackoverflow.com/a/58689472/1148030
open CanvasUtil;
open AnimationUtil;

type canvasState = {frameCount: int, expectedFrameCount: int, evalFunction: Eval.evalFunction};

type canvasEvent = 
  | FrameRendered
  | ExpectedFrameCount(int)
  | SetEvalFunction(Eval.evalFunction);

let initialCanvasState = {frameCount: 0, expectedFrameCount: 0, evalFunction: Eval.initial};

let drawOnCanvas =
    (state: canvasState, dispatchCanvasEvent: (canvasEvent) => unit, context: canvasDrawContext): unit => {
  open Webapi.Canvas.Canvas2d;
  
  if (state.frameCount == 0) {
     clearRect(context.context, ~x=0., ~y=0., ~w=context.dimensions.width, ~h=context.dimensions.height);
  }

  RangeOfInt.map(frame => {
    let t = float_of_int(frame) *. AnimationConstants.targetFrameInterval;
    let renderState: Eval.evalState = state.evalFunction(t);
    setFillStyle(context.context, String, "red");
    let xUnit: unitT = {value: renderState.x /. 2.0 +. 0.5};
    let yUnit: unitT = {value: renderState.y /. 2.0 +. 0.5};
    let x = lerp(0.0, context.dimensions.height, xUnit)
    let y = lerp(context.dimensions.height, 0.0, yUnit)
    fillRect(context.context, ~x=x, ~y=y, ~w=4.0, ~h=4.0);
    dispatchCanvasEvent(FrameRendered);
  }, RangeOfInt.make(state.frameCount, state.expectedFrameCount)) |> ignore;

};

let updateState = (state: canvasState, event: canvasEvent): canvasState => {
  switch(event) {
    | ExpectedFrameCount(c) => {...state, expectedFrameCount: c}
    | FrameRendered => {...state, frameCount: state.frameCount + 1}
    | SetEvalFunction(f) => {evalFunction: f, frameCount: 0, expectedFrameCount: 0}
  }
};

let frameChangeListenerEffect =
    (dispatchCanvasEvent: canvasEvent => unit): ((CodeCanvasState.acceptEvent, unit) => option(unit => unit)) => {
  CodeCanvasState.listenerEffect(stateChange =>
    switch (stateChange) {
    | RenderFramesTo(frame) => dispatchCanvasEvent(ExpectedFrameCount(frame))
    // | SetEvalFunction(frame) => dispatchCanvasEvent(ExpectedFrameCount(frame))
    | _ => ()
    }
  );
};

[@react.component]
let make = () => {
  open React;
  let canvasElementRef: Ref.t(option(Dom.element)) = useRef(None);
  let (canvasState, dispatchCanvasEvent) = React.useReducer(updateState, initialCanvasState);

  useLayoutEffect2(() => {
    Ref.current(canvasElementRef)
    |> Belt.Option.map(_, drawOnCanvasElement(drawOnCanvas(canvasState, dispatchCanvasEvent)))
    |> ignore;
    None;
  }, (false, canvasState.expectedFrameCount));

  React.useEffect0(frameChangeListenerEffect(dispatchCanvasEvent, CodeCanvasState.dispatch));  

  <canvas
    ref={ReactDOMRe.Ref.callbackDomRef(elem =>
      React.Ref.setCurrent(canvasElementRef, Js.Nullable.toOption(elem))
    )}
  />;
};