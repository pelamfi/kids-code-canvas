// https://stackoverflow.com/a/58689472/1148030
open CanvasUtil;
open AnimationUtil;
open Belt;

type canvasState = {frameCount: int, expectedFrameCount: int, scriptletFunction: Scriptlet.scriptletFunction};

type canvasEvent = 
  | FrameRendered
  | Redraw
  | ExpectedFrameCount(int)
  | SetScriptletFunction(Scriptlet.scriptletFunction);

let initialCanvasState = {frameCount: 0, expectedFrameCount: 0, scriptletFunction: Scriptlet.initial};

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
    open MathUtil;  
    let t = float_of_int(frame) *. AnimationConstants.targetFrameInterval;
    let renderState: Scriptlet.scriptletState = state.scriptletFunction(t);
    let xUnit: unitT = wrappingMinusAToPlusAToUnit(renderState.x, widerThanHigher);
    let yUnit: unitT = wrappingMinusAToPlusAToUnit(notchAt1(renderState.y), 1.0);
    let r = abs_float(unitMod(renderState.r *. 0.999999999999)) *. 256.0;
    let g = abs_float(unitMod(renderState.g *. 0.999999999999)) *. 256.0;
    let b = abs_float(unitMod(renderState.b *. 0.999999999999)) *. 256.0;
    let radius = clamp(0.0, w *. 2.0, lerp(0.0, h, {value: renderState.radius}));
    let x = lerp(0.0, w, xUnit);
    let y = lerp(h, 0.0, yUnit);
    setFillStyle(c, String, Printf.sprintf("rgba(%f,%f,%f,%f)", r, g, b, 1.0));
    beginPath(c);
    arc(~x = x, ~y = y, ~r = radius, ~startAngle=0.0, ~endAngle = MathUtil.pi *. 2.0, ~anticw=false, c);
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
    | SetScriptletFunction(f) => {scriptletFunction: f, frameCount: 0, expectedFrameCount: 0}
  }
};

let codeCanvasStateChangeListenerEffect =
    (dispatchCanvasEvent: canvasEvent => unit): ((CodeCanvasState.acceptEvent, unit) => option(unit => unit)) => {
  CodeCanvasState.listenerEffect(stateChange =>
    switch (stateChange) {
    | RenderFramesTo(frame) => dispatchCanvasEvent(ExpectedFrameCount(frame))
    | ScriptletFunctionChanged(evalFunc) => dispatchCanvasEvent(SetScriptletFunction(evalFunc))
    | _ => ()
    }
  );
};

let canvasLayoutProbeDivId = "canvasLayoutProbe";

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
        open Webapi.Dom;
        let e = Document.getElementById(canvasLayoutProbeDivId, document);
        switch(Option.map(e, Element.clientWidth), Ref.current(canvasElementRef)) {
          | (Some(clientWidth), Some(canvasElement)) => {
            let height = float_of_int(clientWidth) *. (9.0 /. 16.0);
            Webapi.Dom.Element.setAttribute("width", string_of_int(clientWidth), canvasElement);
            Webapi.Dom.Element.setAttribute("height", Js.Float.toString(height), canvasElement);
            dispatchCanvasEvent(Redraw);
            ();
          }
          | _ => ()
        };
      };

      let observer = ObserveResize.observeResize(canvasLayoutProbeDivId, sendUpdate);
      Some(() => ObserveResize.unobserve(observer));
    },
    ((), ()),
  );

  React.useEffect0(codeCanvasStateChangeListenerEffect(dispatchCanvasEvent, CodeCanvasState.dispatch));
  <>
  //<div className="canvasWrapperCell">
  //<div className="canvasGrid">
  //<div className="leftCell"/>
  
  <div className="canvasCell">
  <div className="mainCanvasBorder">
  <canvas
    ref={ReactDOMRe.Ref.callbackDomRef(elem =>
      React.Ref.setCurrent(canvasElementRef, Js.Nullable.toOption(elem))
    )}
  />
  </div>
  </div>


  //</div>
  //</div>
  </>;
};