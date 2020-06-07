
[@react.component]
let make = (~loginFunction: (string) => unit) => {

  let (name, nameUpdate) = React.useReducer((_, newName) => newName, "");

  let loginHandler = (_: ReactEvent.Mouse.t) => {
    loginFunction(name)
  };

  let nameChangeHandler = (e: ReactEvent.Form.t) => {
    nameUpdate(e->ReactEvent.Form.target##value)
  };

  <> 
  <div className="loginInputs">
  <label className="loginNamePrompt">
    {React.string("Kirjoita nimesi ja paina \"Aloita\":")}
  </label>
  <input className="loginName" placeholder="Etunimi" onChange=nameChangeHandler/>
  </div>
  <div className="loginButtonWrapper">
    <a className={"loginButton " ++ (name == "" ?  " disabled" : "")} onClick=loginHandler>
      {React.string("Aloita")}
    </a>
  </div>
  </>
};