// https://www.wikiwand.com/en/Modulo_operation
let flooredDivisionRemainder = (a: int, b: int): int => {
  (a mod b + b) mod b;
};

let flooredDivisionRemainderFloat: (float, float) => float = [%bs.raw
  {|
function flooredDivisionRemainderF(a, b) {
  return (a % b + b) % b;
}
|}]

let clampInt = (minValue: int, maxValue: int, value: int): int => max(minValue, min(maxValue, value))

let clamp = (minValue: float, maxValue: float, value: float): float => max(minValue, min(maxValue, value))

let notchAt1 = (value: float): float => value == 1.0 ? 0.999999999999 : value

let pi = [%bs.raw{| Math.PI |}]

// NOTE: This is not properly defined floored division if b < 0
let flooredDivision = (a: int, b: int): int =>
  if (a < 0) {
    (a - (b - 1)) / b;
  } else {
    a / b;
  };

let stringOfIntWithSign = (a: int): string =>
  if (a < 0) {
    string_of_int(a);
  } else if (a > 0) {
    "+" ++ string_of_int(a);
  } else {
    "0";
  };