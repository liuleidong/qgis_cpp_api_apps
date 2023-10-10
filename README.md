# QGis C++ API二次开发示例大全

| 名称 | 描述 |
|--|--|
| ll_qgis_code_examples | 包含大量示例代码：图层添加、矢量栅格数据符号化、MapCanvas使用、Gis算法调用 |
| ll_qgis_realtime_uavs | QGis库二次开发应用，模拟收到经纬度后实时显示在高德地图上，可设置图标等参数 |
| ll_qgis_hello | 最简单的qgis应用，调用QGis库，获取QGis的发行名称 |
| 更多应用陆续添加中...... | |

# 项目特性
- 基于QGis3.28 Qt5.15.3
- 从实际项目中抽取的QGis使用场景
- Apache 协议开源

# 项目文档
- 详细文档见[wiki](https://gitee.com/richie12/qgis_cpp_api_apps/wikis/Home)

# 付费社群
- 这里放上知识星球的图片

# qgis_cpp_api_apps
## ll_qgis_code_examples
### 截图

### 演示QGis C++ API的使用，包括如下功能：
 - [x] 加载各种格式数据
   - [x] shapefile
   - [x] gpx
   - [x] gpkg
   - [x] geojson
   - [x] gml
   - [x] kml
   - [x] dxf
   - [x] coverage目录
   - [x] csv
   - [x] spalialite数据库
   - [x] memory,
   - [x] wfs
   - [x] raster栅格,
   - [x] wms(在线高德，离线高德)
- [x] 地图符号化
    - [x] 矢量点符号化
      - [x] 单一符号（Single symbol）
        - [x] 简单标记（Simple marker）
        - [x] SVG标记（SVG marker）
        - [x] 图片（Raster Image marker）
        - [x] 动画标记（Animated marker）
        - [x] 椭圆形标记（Ellipse marker）
        - [x] 填充标记（Filled marker）
        - [x] 字符标记（Font marker）
        - [x] 几何生成器（Geometry generator）
        - [x] 掩膜（Mask marker）
        - [x] 矢量字段标记（Vector field marker）
      - [x] 分类（Categorized）
      - [x] 分级（Graduated）
      - [x] 基于规则（Rule-based）
      - [x] 点分布（Point displacement）
      - [x] 点聚合（Point cluster）
      - [x] 热力图（Heatmap）
    - [x] 矢量线符号化
    - [x] 矢量面符号化
1. 矢量线符号化包括单一符号(Simple Line,Arrow,Interpolated Line,Hashed Line,Marker Line,Raster Line,Linebrust)
2. 矢量面符号化包括单一符号(Simple fill,Centroid fill,Gradient fill,Line pattern fill,Point pattern fill,Raster image fill,Svg fill,Random Marker fill,Shapebrust fill,Outline:Simple line,Outline:Arrow,Outline:Hashed line,Outline:Interpolated,Outline:Linebrust,Outline:Marker line,Outline:Raster line)，反转面要素(Inverted polygons)，2.5D渲染
3. 栅格符号化包括多波段彩色(Multiband color)，唯一值着色(Patelled/Unique)，单波段灰度(Singleband gray)，多波段伪彩色(Signleband pseudocolor)，山体阴影(Hillshade)，轮廓(Contours)
4. 地图标注(Label)，注释图层(Annoation Layer)，文字类型注释，点类型注释，线类型注释，多边形类型注释
5. 地图图表包括饼图(Pie Diagram)、文本图(Text Diagram)、直方图(Histogram)以及分段条形图(StackedBar Diagram)
6. 矢量图层包含的数据显示，数据选择，数据查询，空间索引，距离面积计算以及将图层写入文件
7.  地图Canvas的使用包括背景色，中心点，角度的修改，地图工具(Map Pan，Map Zoom)的使用，RubberBand画线画多边形，VertexMarker的使用
8.  图层树的使用，在图层叶子节点右键菜单的实现
9.  内置算法的使用(随机生成点、按照参照面裁剪、Buffer)
### 运行截图

## ll_qgis_realtime_uavs
演示从无人机获取数据，在地图上显示实时轨迹

### 运行截图

## ll_qgis_hello
1. 最简单的qgis应用，获取qgis的发行名称并显示在界面上

### 运行截图