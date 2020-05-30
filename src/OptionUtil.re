
let getWithDefaultF = (defaultValueFunc: unit => 'a, option: option('a)): 'a => {
    switch(option) {
    | Some(value) => value
    | None => defaultValueFunc()
    }
};