// https://stackoverflow.com/a/58689472/1148030
open CanvasUtil;
open AnimationUtil;
open Belt;

type canvasState = {frameCount: int, expectedFrameCount: int, evalFunction: Eval.evalFunction};

type canvasEvent = 
  | FrameRendered
  | Redraw
  | ExpectedFrameCount(int)
  | SetEvalFunction(Eval.evalFunction);

let initialCanvasState = {frameCount: 0, expectedFrameCount: 0, evalFunction: Eval.initial};

let drawOnCanvas =
    (state: canvasState, dispatchCanvasEvent: (canvasEvent) => unit, context: canvasDrawContext): unit => {
  open Webapi.Canvas.Canvas2d;
  
  let c = context.context;
  let h = context.dimensions.height;
  let w = context.dimensions.width;

  if (state.frameCount == 0) {
     clearRect(c, ~x=0., ~y=0., ~w=w, ~h=h);
  }

  let widerThanHigher = w /. h;

  let frameRenderCount = MathUtil.clampInt(0, 60, state.expectedFrameCount - state.frameCount);

  RangeOfInt.map(frame => {
    let t = float_of_int(frame) *. AnimationConstants.targetFrameInterval;
    let renderState: Eval.evalState = state.evalFunction(t);
    let xUnit: unitT = {value: MathUtil.flooredDivisionRemainderFloat(renderState.x /. 2.0 +. 0.5 *. widerThanHigher, widerThanHigher)};
    let yUnit: unitT = {value: MathUtil.flooredDivisionRemainderFloat(renderState.y /. 2.0 +. 0.5, 1.0)};
    let r = unitMod(renderState.r) *. 256.0;
    let g = unitMod(renderState.g) *. 256.0;
    let b = unitMod(renderState.b) *. 256.0;
    let x = lerp(0.0, h, xUnit);
    let y = lerp(h, 0.0, yUnit);
    let f = w /. 100.0
    setFillStyle(c, String, Printf.sprintf("rgba(%f,%f,%f,%f)", r, g, b, 1.0));
    beginPath(c);
    arc(~x = x, ~y = y, ~r = f, ~startAngle=0.0, ~endAngle = MathUtil.pi *. 2.0, ~anticw=false, c);
    fill(c);
    // fillRect(c, ~x=x, ~y=y, ~w=4.0, ~h=4.0);
    dispatchCanvasEvent(FrameRendered);
  }, RangeOfInt.make(state.frameCount, state.frameCount + frameRenderCount)) |> ignore;
};

let updateState = (state: canvasState, event: canvasEvent): canvasState => {
  switch(event) {
    | Redraw => {...state, frameCount: 0}
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

let canvasCellId = "mainCanvas";

[@react.component]
let make = () => {
  open React;
  let canvasElementRef: Ref.t(option(Dom.element)) = useRef(None);
  let (canvasState, dispatchCanvasEvent) = React.useReducer(updateState, initialCanvasState);

  useLayoutEffect2(() => {
    Ref.current(canvasElementRef)
    |> Option.map(_, drawOnCanvasElement(drawOnCanvas(canvasState, dispatchCanvasEvent)))
    |> ignore;
    None;
  }, (false, canvasState.expectedFrameCount));

  React.useLayoutEffect2(
    () => {
      let sendUpdate = () => {
        let doc = Webapi.Dom.document;
        let e = Webapi.Dom.Document.getElementById(canvasCellId, doc);
        switch(Option.map(e, Webapi.Dom.Element.getBoundingClientRect), Ref.current(canvasElementRef)) {
          | (Some(clientRect), Some(canvasElement)) => {
            let width = Webapi.Dom.DomRect.width(clientRect);
            let height = width *. (9.0 /. 16.0);
            Webapi.Dom.Element.setAttribute("width", Js.Float.toString(width), canvasElement);
            Webapi.Dom.Element.setAttribute("height", Js.Float.toString(height), canvasElement);
            dispatchCanvasEvent(Redraw);
            ();
          }
          | _ => ()
        };
      };

      let observer = ObserveResize.observeResize(canvasCellId, sendUpdate);
      Some(() => ObserveResize.unobserve(observer));
    },
    ((), ()),
  );


  React.useEffect0(frameChangeListenerEffect(dispatchCanvasEvent, CodeCanvasState.dispatch));
  <>
  <div className="canvasRow">
  <div className="leftCell"/>
  
  <div className="canvasCell">
  <canvas
    ref={ReactDOMRe.Ref.callbackDomRef(elem =>
      React.Ref.setCurrent(canvasElementRef, Js.Nullable.toOption(elem))
    )}
  />
  </div>

  <div className="rightCell"/>
  </div>

  <div className="canvasProbeRow">
  <div id={canvasCellId} className="canvasProbeCell">
  </div>
  </div>  
  </>;
};