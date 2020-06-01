

[@react.component]
let make = () => {
  <div className="helpCell">
  <dl>
  <dt>{React.string("x")}</dt><dd>{React.string("on sijainti x-akselilla, -1.0 on vasemmalla, 1.0 on oikealla.")}</dd>
  <dt>{React.string("y")}</dt><dd>{React.string("on sijainti x-akselilla, -1.0 on vasemmalla, 1.0 on oikealla.")}</dd>
  </dl>
  </div>
};