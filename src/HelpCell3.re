

[@react.component]
let make = () => {
  <div className="helpCell">
  <dl>
  <dt>{React.string("punainen")}</dt><dd>{React.string({js|punaisen värin osuus 0:sta 1:een|js})}</dd>
  <dt>{React.string("sininen")}</dt><dd>{React.string({js|sinisen värin osuus 0:sta 1:een|js})}</dd>
  <dt>{React.string({js|vihreä|js})}</dt><dd>{React.string({js|vihrean värin osuus 0:sta 1:een|js})}</dd>
  <dt>{React.string({js|koko|js})}</dt><dd>{React.string({js|pallon koko|js})}</dd>
  </dl>
  </div>
};