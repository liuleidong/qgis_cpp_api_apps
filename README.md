# QGis C++ API二次开发示例大全

| 名称 | 描述 |
|--|--|
| ll_qgis_code_examples | 包含大量示例代码：图层添加、地图符号化、MapCanvas使用、Gis算法调用等等 |
| ll_qgis_realtime_uavs | 模拟收到经纬度后实时显示在高德地图上，可设置图标等参数 |
| ll_qgis_hello | 最简单的qgis应用，调用QGis库，获取QGis的发行名称 |
| 更多应用陆续添加中...... | |

# 项目特性
- 基于QGis3.28 Qt5.15.3
- 从实际项目中抽取的QGis二次开发使用场景
- 教程中有QGis软件操作和二次开发代码对比
- Apache 协议开源

# 项目文档
- 详细文档见[wiki](https://gitee.com/richie12/qgis_cpp_api_apps/wikis/Home)
- [QGIS离线文档](https://gitee.com/richie12/qgis_cpp_api_apps/wikis/QGis%E7%A6%BB%E7%BA%BF%E6%96%87%E6%A1%A3)
- [QGis库架构](https://gitee.com/richie12/qgis_cpp_api_apps/wikis/QGis%E5%BA%93%E6%95%B4%E4%BD%93%E6%9E%B6%E6%9E%84)
- [QGis调试环境搭建](https://gitee.com/richie12/qgis_cpp_api_apps/wikis/QGis%E8%B0%83%E8%AF%95%E7%8E%AF%E5%A2%83%E6%90%AD%E5%BB%BA)
- [编译](https://gitee.com/richie12/qgis_cpp_api_apps/wikis/%E7%BC%96%E8%AF%91)
- [运行](https://gitee.com/richie12/qgis_cpp_api_apps/wikis/%E8%BF%90%E8%A1%8C)

# 社群
## 公众号
![输入图片说明](https://foruda.gitee.com/images/1697077286578350399/c111e1c7_1547275.jpeg "qrcode_for_gh_5fe62453ec05_258.jpg")
- 请关注微信公众号获取最新消息推送
## QQ群
![输入图片说明](https://foruda.gitee.com/images/1699751272170435321/86a409ca_1547275.png "屏幕截图")
- 加入qq群
## 知识星球
![输入图片说明](https://foruda.gitee.com/images/1697160230025579811/496ec4a9_1547275.png "屏幕截图")
- 加入知识星球有偿咨询
# 源码说明
## ll_qgis_code_examples
### 截图
- 添加离线高德影像
![输入图片说明](https://foruda.gitee.com/images/1697005305558159268/a0e65779_1547275.gif "1.gif")
- RubberBand画多边形
![输入图片说明](https://foruda.gitee.com/images/1697005324434196244/cadd0c0e_1547275.gif "2.gif")
- 符号化-点-动画标记
![输入图片说明](https://foruda.gitee.com/images/1697005336901985866/7e59e5b8_1547275.gif "3.gif")
- 符号化-面-2.5D
![输入图片说明](https://foruda.gitee.com/images/1697005349623295156/f3f6ff15_1547275.gif "4.gif")
- 地图图表
![输入图片说明](https://foruda.gitee.com/images/1697005363624765326/5c3caf6b_1547275.gif "5.gif")
- 数据处理-算法-裁剪-随机点生成-缓冲区
![输入图片说明](https://foruda.gitee.com/images/1697005378968366819/b450afff_1547275.gif "6.gif")

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
      - [x] 简单线（Simple line）
      - [x] 栅格线（Raster Line）
      - [x] 箭头（Arrow）
      - [x] 几何生成器（Geometry generator）
      - [x] 短划线（Hashed Line）
      - [x] 插值线（Interpolated Line）
      - [x] 爆炸线（Lineburst）
      - [x] 标记线（Marker line）
    - [x] 矢量面符号化
      - [x] 单一符号（Single symbol）
        - [x] 仅渲染中心点（Centroid fill）
    	- [x] 几何生成器（Geometry generator）
    	- [x] 渐变填充（Gradient fill）
    	- [x] 使用线符号填充（Line pattern fill）
    	- [x] 使用点符号填充（Point pattern fill）
    	- [x] 随机标记填充（Random Marker Fill）
    	- [x] 栅格数据填充（Raster image fill）
    	- [x] SVG填充（SVG fill）
    	- [x] 形状炸裂填充（Shapeburst fill）
    	- [x] 简单填充（Simple fill）
    	- [x] 箭头渲染边界（Outline: Arrow）
    	- [x] 短划线渲染边界（Outline: Hashed Line）
    	- [x] 插值线渲染边界（Outline: Interpolated Line）
    	- [x] 爆炸线渲染边界（Outline: Lineburst）
    	- [x] 标记符号渲染边界（Outline: Marker line）
    	- [x] 栅格线渲染边界（Outline: Raster Line）
    	- [x] 简单线渲染边界（Outline: Simple line）
      - [x] 反转面要素（Inverted polygons）
      - [x] 2.5 D
    - [x] 栅格数据符号化
    	- [x] Multiband color：多波段彩色
    	- [x] Paletted/Unique values：唯一值着色
    	- [x] Singleband gray：单波段灰度
    	- [x] Singleband pseudocolor：单波段伪彩色
    	- [x] Hillshade：山体阴影
    	- [x] Contours：轮廓
- [x] 地图标注，地图注释
- [x] 地图图表
  - [x] 饼图(Pie Diagram)
  - [x] 文本图(Text Diagram)
  - [x] 直方图(Histogram)
  - [x] 分段条形图(StackedBar Diagram)
- [x] 矢量数据操作:数据显示，数据选择，数据查询，空间索引，距离面积计算以及将图层写入文件
- [x] MapCanvas使用
  - [x] 地图Canvas的使用:背景色，中心点，角度的修改
  - [x] 地图工具(Map Pan，Map Zoom)的使用
  - [x] RubberBand画线画多边形
  - [x] VertexMarker的使用
  - [x] 图层树的使用，在图层叶子节点右键菜单的实现
- [x] 内置算法的使用
  - [x] 随机生成点
  - [x] 按照参照面裁剪
  - [x] Buffer缓冲区

## ll_qgis_realtime_uavs
演示从无人机获取数据，在地图上显示实时轨迹
### 截图
![输入图片说明](https://foruda.gitee.com/images/1696998704428412332/0f6c662d_1547275.gif "uav.gif")
### 功能说明
- [x] 加载高德路网地图
- [x] 模拟接收设备经纬度
- [x] 创建memory图层
- [x] 显示设备实时位置
- [x] 显示设备实时轨迹
- [x] 设置设备符号，可设置简单点符号或者svg符号
- [x] 设置其他属性
- [ ] 轨迹线颜色
- [ ] 轨迹线宽度

## ll_qgis_hello
### 截图
![输入图片说明](https://foruda.gitee.com/images/1696998146067525015/c9785510_1547275.png "屏幕截图")
### 功能说明
- [x] 简单的hello qgis应用
- [x] 获取qgis的发行名称并显示在界面上

# 参考资料
- [A gentle introduction in GIS](https://docs.qgis.org/3.28/en/docs/gentle_gis_introduction/)
- [PyQGIS cookbook](https://docs.qgis.org/3.28/en/docs/pyqgis_developer_cookbook/)
- [GIS Training manual](https://docs.qgis.org/3.28/en/docs/training_manual/)
- [超图examples](https://iclient.supermap.io/examples/leaflet/examples.html#iPortal)
- [QGIS软件及其应用教程](https://e.jd.com/30710101.html?ebook=1)