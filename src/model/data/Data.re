open DataType;

let stateData = {state: None, isDebug: true};

let getStateData = () => stateData;

let unsafeGetStateByThrow = () => getStateData().state |> Option.unsafeGetByThrow;

let setState = state => {
  getStateData().state = Some(state);

  state;
};