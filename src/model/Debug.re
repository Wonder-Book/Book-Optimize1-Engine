open DataType;

let getIsDebug = stateData => stateData.isDebug;

let setIsDebug = (stateData, isDebug) => {
  stateData.isDebug = isDebug;

  stateData;
};