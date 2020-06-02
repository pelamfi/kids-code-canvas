

[@react.component]
let make = () => {
  <div className="helpCell">
  <dl>
  <dt>{React.string("y")}</dt><dd>{React.string("on sijainti y-akselilla, -1.0 on vasemmalla, 1.0 on oikealla.")}</dd>
  <dt>{React.string("x")}</dt><dd>{React.string("on sijainti x-akselilla, vasen -1.77 on vasemmalla, 1.77 (oikea) on oikealla.")}</dd>
  </dl>
  </div>
};