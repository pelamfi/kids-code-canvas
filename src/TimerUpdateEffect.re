
type effectCleanup = unit => unit;

type effect = unit => option(effectCleanup);

let timerUpdateEffect = (isAnimating: bool, dispatch: (CodeCanvasState.event) => unit): effect => {
  () =>
    if (isAnimating) {
      Some(GroupedRaf.register((timerMs: float) => dispatch(AnimationFrame(timerMs))));
    } else {
      None;
    };
};