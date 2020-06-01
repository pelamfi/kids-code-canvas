

[@react.component]
let make = () => {
  <div className="helpCell">
  <dl>
  <dt>{React.string("punainen")}</dt><dd>{React.string({js|punaisen värin osuus|js})}</dd>
  <dt>{React.string("sininen")}</dt><dd>{React.string({js|sinisen värin osuus|js})}</dd>
  <dt>{React.string("vihrea")}</dt><dd>{React.string({js|vihrean värin osuus|js})}</dd>
  </dl>
  </div>
};