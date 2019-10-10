# benchmark test: show 7000 triangles



# find problem:
profile
timeline?
memory


# optimize

(optimize each one and compare benchmark)


## optimize initAll

optimize:
get shader info only when debug


why set "isDebug" to state data instead of state?
(and why mutable?)





## optimize loop


- cache location
////utils: add sparse map








- set camera data only once

////(split uniform data to camera data, model data)



- cache uniform data
not all uniform data need cache
(e.g. matrix data not cache; array number(int/float) data can cache)



/*
need test difference perf!!!

add MutableHashMap for performance:
uniformCacheMap, shaderCacheMap is MutableHashMap

*/




discuss whether to reduce state in Render->render iterate?
now reduce it instead of:
1.reduce small data(e.g. uniformCacheMap)
2.use mutable(e.g. change shaderCacheMap to mutable hashMap)


because reduce state has these advantage:
1.improve code maintainablity
    unify to reduce one state, instead of many small data;
    immutable data has little bug than mutable data;


because reduce state has these disadvantage:
1.bad performance
spend more time for gc




- cache use program


- use vao(extension)

judge last vao
