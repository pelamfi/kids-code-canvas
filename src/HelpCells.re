

[@react.component]
let make = () => {
  <React.Fragment>

  <div className="helpCell1">
  <dl>
  <dt>{React.string("x")}</dt><dd>{React.string("on sijainti x-akselilla, vasen -1.77 on vasemmalla, 1.77 (oikea) on oikealla.")}</dd>
  <dt>{React.string("y")}</dt><dd>{React.string("on sijainti y-akselilla, -1.0 on vasemmalla, 1.0 on oikealla.")}</dd>
  <dt>{React.string("t")}</dt><dd>{React.string({js|on aika sekunteina joka alkaa nollasta ja menee itsestään eteenpäin.|js})}</dd>
  </dl>
  </div>

  <div className="helpCell">
  <dl>
  <dt>{React.string("aalto(t)")}</dt><dd>{React.string({js|on aalto nollan ja yhden välillä. Esimerkiksi aalto(t) tuottaa aaltoilua kerran sekunnissa.|js})}</dd>
  <dt>{React.string("sin(t)")}</dt><dd>{React.string({js|on sinifunktio. Esimerkiksi sin(t) tuottaa aaltoilua -1 ja 1 välillä|js})}</dd>
  <dt>{React.string("cos(t)")}</dt><dd>{React.string({js|on kosinifunktio. Esimerkiksi cos(t) tuottaa aaltoilua -1 ja 1 välillä|js})}</dd> 
  <dt>{React.string("random()")}</dt><dd>{React.string({js|on satunnainen luku nollan ja yhden väliltä|js})}</dd> 
  <dt>{React.string("if")}</dt><dd>{React.string({js|on ehtolausekkeen alku. Esimerkiksi: if (t > 1.0) y = t|js})}</dd> 
  </dl>
  </div>

  <div className="helpCell">
  <dl>
  <dt>{React.string({js|koko|js})}</dt><dd>{React.string({js|pallon koko|js})}</dd>
  <dt>{React.string("punainen")}</dt><dd>{React.string({js|punaisen värin osuus 0:sta 1:een|js})}</dd>
  <dt>{React.string("sininen")}</dt><dd>{React.string({js|sinisen värin osuus 0:sta 1:een|js})}</dd>
  <dt>{React.string({js|vihreä|js})}</dt><dd>{React.string({js|vihrean värin osuus 0:sta 1:een|js})}</dd>
  <dt>{React.string({js|läpinäkyvyys|js})}</dt><dd>{React.string({js|0 on ei läpinäkyvä, 0.9 on melkein läpinäkyvä.|js})}</dd>
  </dl>
  </div>

  </React.Fragment>
};