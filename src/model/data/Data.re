open DataType;

let stateData = {state: None, isDebug: true};

let getStateData = () => stateData;

let unsafeGetState = () => getStateData().state |> Option.unsafeGet;

let setState = state => {
  getStateData().state = Some(state);

  state;
};