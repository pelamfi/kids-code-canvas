

[@react.component]
let make = () => {
  <div className="helpCell">
  <dl>
  <dt>{React.string("t")}</dt><dd>{React.string({js|on aika sekunteina joka alkaa nollasta ja menee itsestään eteenpäin.|js})}</dd>
  <dt>{React.string("sin")}</dt><dd>{React.string({js|on sinifunktio. Eximerkiksi sin(t) tuottaa aaltoilua -1 ja 1 välillä|js})}</dd>
  <dt>{React.string("cos")}</dt><dd>{React.string({js|on kosinifunktio. Eximerkiksi cos(t) tuottaa aaltoilua -1 ja 1 välillä|js})}</dd> 
  <dt>{React.string("if")}</dt><dd>{React.string({js|on ehtolausekkeen alku. Esimerkiksi: if (t > 1.0) y = t|js})}</dd> 
  </dl>
  </div>
};