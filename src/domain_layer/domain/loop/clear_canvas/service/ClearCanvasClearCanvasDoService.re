let clearCanvas = (): ResultContainerVO.t(unit, Js.Exn.t) => {
  ContextContextEntity.getGl()
  |> ResultContainerVO.mapSuccess(gl => {
       ContextContextEntity.clearCanvas(gl)
     });
};