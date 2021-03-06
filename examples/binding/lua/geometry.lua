-------------------------------------------------------
-- import TerraLib module and do some initialization --
-------------------------------------------------------
package.cpath = package.cpath .. ";./lib?.so"

print(package.cpath)

require ( "terralib_mod_binding_lua" )
 
---------------------------
-- using Geometry module --       
---------------------------

--t = terralib_mod_binding_lua.TeSingleton.getInstance()

--t:initialize() 

pt = terralib_mod_binding_lua.te.gm.Point(74.2, 23.5, 4326)

print("X = " .. pt:getX())
print("Y = " .. pt:getY())
print("dimension = " .. pt:getDimension())
print("coord dimension = " .. pt:getCoordinateDimension())
print("geom type = " .. pt:getGeometryType())
print("srid = " .. pt:getSRID())
pt:setSRID(4128)
print("srid = " .. pt:getSRID())
pt:setSRID(4326)
--pt:transform(4128)
mbrPoly = pt:getEnvelope()
mbr = pt:getMBR()
--pt:computeMBR()
pt:computeMBR(true)
pt:computeMBR(false)
print("wkt = " .. pt:asText())
k = 0
print("wkb = " .. pt:asBinary(k))
print("wkb size = " .. pt:getWkbSize())
wkb = 0
--print("wkb NDR (little endian) = " .. pt:getWkb(wkb, terralib_mod_binding_lua.te.common.NDR))
--print("wkb XDR (big endian) = " .. pt:getWkb(wkb, terralib_mod_binding_lua.te.common.XDR))
print("isEmpty? ")
print(pt:isEmpty())
print("isSimple? ")
print(pt:isSimple())
print("isValid? ")
print(pt:isValid())
print("is3D? ")
print(pt:is3D())
print("isMeasured? ")
print(pt:isMeasured())
boundary = pt:getBoundary()
npts = pt:getNPoints()
print("npts = " .. npts)
print("equals? ")
print(pt:equals(pt))
print("disjoint? ")
print(pt:disjoint(pt))
print("intersects? ")
print(pt:intersects(pt))
print("touches? ")
print(pt:touches(pt))
print("crosses? ")
print(pt:crosses(pt))
print("within? ")
print(pt:within(pt))
print("contains? ")
print(pt:contains(pt))
print("overlaps? ")
print(pt:overlaps(pt))
print("relate? ")
print(pt:relate(pt, "ttttttttt"))
--print("relation? ")
--print(pt:relation(pt))
print("covers? ")
print(pt:covers(pt))
print("covered by? ")
print(pt:coveredBy(pt))

pt:setX(12.0)
print("X = " .. pt:getX())
pt:setY(18.0)
print("Y = " .. pt:getY())

--t:finalize()
