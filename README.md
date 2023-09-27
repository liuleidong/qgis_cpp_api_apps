# qgis_cpp_api_apps
使用QGis C++ API进行二次开发的一些app
- ll_qgis_demos gis常用功能演示
- ll_qgis_realtime_uavs 接收无人机经纬度信息，实时显示在地图上

# 项目特性
- 涵盖了很多实际使用场景
- Apache 协议开源

# 项目文档

# 付费社群

## ll_qgis_demos
演示QGis C++ API的使用，包括如下功能：
1. 加载各种格式图层，包括shapefile,gpx,gpkg,geojson,gml,kml,dxf,coverage目录,csv,spalialite数据库,memory,wfs,tiff,wms(在线高德，离线高德)
2. 矢量点符号化包括单一符号(Simple Marker,Svg Marker,Raster Marker,Animated Marker,Ellipse Marker,Font Marker,Filled Marker,Geometry Generator Marker,Vector Field Marker,Mask)，分类渲染(Categorized)，分级渲染(Graduated)，基于规则渲染(Rule-based)，点分布(Point displacement)，点聚合(Point cluster)，热力图(Heatmap)
3. 矢量线符号化包括单一符号(Simple Line,Arrow,Interpolated Line,Hashed Line,Marker Line,Raster Line,Linebrust)
4. 矢量面符号化包括单一符号(Simple fill,Centroid fill,Gradient fill,Line pattern fill,Point pattern fill,Raster image fill,Svg fill,Random Marker fill,Shapebrust fill,Outline:Simple line,Outline:Arrow,Outline:Hashed line,Outline:Interpolated,Outline:Linebrust,Outline:Marker line,Outline:Raster line)，反转面要素(Inverted polygons)，2.5D渲染
5. 栅格符号化包括多波段彩色(Multiband color)，唯一值着色(Patelled/Unique)，单波段灰度(Singleband gray)，多波段伪彩色(Signleband pseudocolor)，山体阴影(Hillshade)，轮廓(Contours)
6. 地图标注(Label)，注释图层(Annoation Layer)，文字类型注释，点类型注释，线类型注释，多边形类型注释
7. 地图图表包括饼图(Pie Diagram)、文本图(Text Diagram)、直方图(Histogram)以及分段条形图(StackedBar Diagram)
8. 矢量图层包含的数据显示，数据选择，数据查询，空间索引，距离面积计算以及将图层写入文件
9. 地图Canvas的使用包括背景色，中心点，角度的修改，地图工具(Map Pan，Map Zoom)的使用，RubberBand画线画多边形，VertexMarker的使用
10. 图层树的使用，在图层叶子节点右键菜单的实现
11. 内置算法的使用(随机生成点、按照参照面裁剪、Buffer)

### 项目运行截图

## ll_qgis_realtime_uavs
演示从无人机获取数据，在地图上显示实时轨迹

### 项目运行截图