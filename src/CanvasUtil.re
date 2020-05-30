// https://stackoverflow.com/a/58689472/1148030
type canvasDimensions = {
  width: float,
  height: float,
};

type canvasDrawContext = {
  canvasElement: Dom.element,
  context: Webapi.Canvas.Canvas2d.t,
  dimensions: canvasDimensions,
};

type canvasDraw = canvasDrawContext => unit;

let getCanvasDimensions = (canvasElement: Dom.element): canvasDimensions =>
  Webapi.Canvas.CanvasElement.{
    width: float_of_int(width(canvasElement)),
    height: float_of_int(height(canvasElement)),
  };

let drawOnCanvasElement = (draw: canvasDraw, canvasElement: Dom.element): unit => {
  let canvasDrawContext: canvasDrawContext =
    Webapi.Canvas.CanvasElement.{
      canvasElement,
      context: getContext2d(canvasElement),
      dimensions: getCanvasDimensions(canvasElement),
    };

  draw(canvasDrawContext);
};

let checkOffscreenCanvasSupport: unit => bool = [%bs.raw {|
function checkOffscreenCanvasSupport() {
    if (typeof document.createElement('canvas').transferControlToOffscreen  === "function") {
        return true;
    } else {
        return false;
    }
}
|}]

// https://stackoverflow.com/a/56884980/1148030
let isOffscreenCanvasSupported: bool = checkOffscreenCanvasSupport();

let drawOffscreenCanvasElement: (Dom.element, Webapi.Canvas.Canvas2d.t) => unit = [%bs.raw {|
function renderSingleNoteOnStaffInternal(sourceElement, targetContext) {
    targetContext.drawImage(sourceElement, 0, 0);
}
|}];

let drawOffscreenCanvas = (source: canvasDrawContext, target: canvasDrawContext): unit => {
    drawOffscreenCanvasElement(source.canvasElement, target.context);
};

let getOffscreenCanvas = (canvasDimensions: canvasDimensions): option(canvasDrawContext) => {
    if (isOffscreenCanvasSupported) {
        let canvasElement = Webapi.Dom.Document.createElement("canvas", Webapi.Dom.document);
        Some(Webapi.Canvas.CanvasElement.{
            canvasElement,
            context: getContext2d(canvasElement),
            dimensions: canvasDimensions,
        });
    } else {
        None
    }
}


