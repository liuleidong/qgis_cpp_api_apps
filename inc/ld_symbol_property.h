#ifndef LDCOMMON_H
#define LDCOMMON_H

#include <QString>
#include <QColor>
#include <QMetaObject>
#include <QMetaProperty>

/// LLD_TODO Geometry generator/Mask/Vector Field Marker使用
///
typedef struct _SMarkerSymbolBase {
    Q_GADGET
    Q_PROPERTY(QString offset MEMBER offset)
    Q_PROPERTY(QString offset_unit MEMBER offset_unit)
    Q_PROPERTY(QString offset_map_unit_scale MEMBER offset_map_unit_scale)
    Q_PROPERTY(QString size MEMBER size)
    Q_PROPERTY(QString size_unit MEMBER size_unit)
    Q_PROPERTY(QString size_map_unit_scale MEMBER size_map_unit_scale)
    Q_PROPERTY(QString horizontal_anchor_point MEMBER horizontal_anchor_point)
    Q_PROPERTY(QString vertical_anchor_point MEMBER vertical_anchor_point)
public:
    QString size = QStringLiteral("1.0");   //大小
    QString size_unit;  //size单位 可取值同offset_unit
    QString size_map_unit_scale; //同offset_map_unit_scale
    QString offset = QStringLiteral("0"); //偏移量
    QString offset_unit; //偏移量单位 MM(毫米) 可取值如下 RenderMetersInMapUnits/meters/MapUnit/mapunits/Pixel/Percentage/percent/Point/points/Inch
    QString offset_map_unit_scale; //设置offset的最大最小scale 形式如下3x:%1,%2,%3,%4,%5,%6 见QgsSymbolLayerUtils::encodeMapUnitScale
    QString horizontal_anchor_point = QStringLiteral("0");//水平锚点 可取值0(Left)/1(HCenter)/2(Right)
    QString vertical_anchor_point = QStringLiteral("0");//垂直锚点 可取值0(Top)/1(VCenter)/2(Bottom)
}SMarkerSymbolBase;

///简单标记（Simple marker）
typedef struct _SMarkerSymbolSimple : public SMarkerSymbolBase{
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString color MEMBER color)
    Q_PROPERTY(QString outline_color MEMBER outline_color)
    Q_PROPERTY(QString joinstyle MEMBER joinstyle)
    Q_PROPERTY(QString angle MEMBER angle)
    Q_PROPERTY(QString scale_method MEMBER scale_method)
    Q_PROPERTY(QString outline_style MEMBER outline_style)
    Q_PROPERTY(QString outline_width MEMBER outline_width)
    Q_PROPERTY(QString outline_width_unit MEMBER outline_width_unit)
    Q_PROPERTY(QString outline_width_map_unit_scale MEMBER outline_width_map_unit_scale)
    Q_PROPERTY(QString cap_style MEMBER cap_style)
public:
    QString name = QStringLiteral("circle");   //形状 具体形状见 qgis.h MarkerShape枚举
    QString color = QStringLiteral("red");  //颜色 支持red/yellow等颜色名称 也支持255,255,255 rgb形式的颜色
    QString outline_color; //边框颜色 同上
    QString joinstyle;  //相交效果可取值为bevel/miter/round  QgsSymbolLayerUtils::decodePenJoinStyle
    QString angle;      //角度
    QString scale_method;  //可取值为diameter area
    QString outline_style; //线型可取值如下 no/solid/dash/dot/dash dot/dash dot dot
    QString outline_width; //边框宽度
    QString outline_width_unit; //边框宽度单位 可取值同offset_unit
    QString outline_width_map_unit_scale;//同offset_map_unit_scale
    QString cap_style;//PenCapStyle 可取值square/flat/round
}SMarkerSymbolSimple;

///SVG标记（SVG marker）
typedef struct _SMarkerSymbolSvg : public SMarkerSymbolBase{
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString angle MEMBER angle)
    Q_PROPERTY(QString scale_method MEMBER scale_method)
    Q_PROPERTY(QString fixedAspectRatio MEMBER fixedAspectRatio)
    Q_PROPERTY(QString color MEMBER color)
    Q_PROPERTY(QString outline_color MEMBER outline_color)
    Q_PROPERTY(QString outline_width MEMBER outline_width)
    Q_PROPERTY(QString outline_width_unit MEMBER outline_width_unit)
    Q_PROPERTY(QString outline_width_map_unit_scale MEMBER outline_width_map_unit_scale)
public:
    QString name;       //svg文件路径
    QString angle;      //角度
    QString scale_method;  //可取值为diameter area
    QString fixedAspectRatio;   //
    QString color;  //颜色 颜色支持见 QgsSymbolLayerUtils::decodeColor
    QString outline_color; //边框颜色
    QString outline_width; //边框宽度
    QString outline_width_unit; //边框宽度单位
    QString outline_width_map_unit_scale;//同size_map_unit_scale
}SMarkerSymbolSvg;

///栅格数据标记（Raster image marker）
typedef struct _SMarkerSymbolRaster : public SMarkerSymbolBase{
    Q_GADGET
    Q_PROPERTY(QString imageFile MEMBER imageFile)
    Q_PROPERTY(QString angle MEMBER angle)
    Q_PROPERTY(QString scale_method MEMBER scale_method)
public:
    QString imageFile;
    QString angle;
    QString scale_method;
}SMarkerSymbolRaster;

///动态图标记（Animated marker）
typedef struct _SMarkerSymbolAnimated : public SMarkerSymbolRaster{
    Q_GADGET
    Q_PROPERTY(QString frameRate MEMBER frameRate)
public:
    QString frameRate = "10";
}SMarkerSymbolAnimated;

///椭圆形标记（Ellipse marker）
typedef struct _SMarkerSymbolEllipse : public SMarkerSymbolBase{
    Q_GADGET
    Q_PROPERTY(QString symbol_name MEMBER symbol_name)
    Q_PROPERTY(QString symbol_width MEMBER symbol_width)
    Q_PROPERTY(QString symbol_width_unit MEMBER symbol_width_unit)
    Q_PROPERTY(QString symbol_width_map_unit_scale MEMBER symbol_width_map_unit_scale)
    Q_PROPERTY(QString symbol_height MEMBER symbol_height)
    Q_PROPERTY(QString symbol_height_unit MEMBER symbol_height_unit)
    Q_PROPERTY(QString symbol_height_map_unit_scale MEMBER symbol_height_map_unit_scale)
    Q_PROPERTY(QString angle MEMBER angle)
    Q_PROPERTY(QString outline_style MEMBER outline_style)
    Q_PROPERTY(QString joinstyle MEMBER joinstyle)
    Q_PROPERTY(QString cap_style MEMBER cap_style)
    Q_PROPERTY(QString outline_width MEMBER outline_width)
    Q_PROPERTY(QString outline_width_unit MEMBER outline_width_unit)
    Q_PROPERTY(QString outline_width_map_unit_scale MEMBER outline_width_map_unit_scale)
    Q_PROPERTY(QString color MEMBER color)
    Q_PROPERTY(QString outline_color MEMBER outline_color)

public:
    QString symbol_name;//circle/square/rectangle/diamond/cross/arrow/half_arc/triangle/right_half_triangle/left_half_triangle
                        //semi_circle/third_circle/quarter_circle/pentagon/hexagon/octagon/star
    QString symbol_width;
    QString symbol_width_unit;
    QString symbol_width_map_unit_scale;
    QString symbol_height;
    QString symbol_height_unit;
    QString symbol_height_map_unit_scale;
    QString angle;
    QString outline_style;
    QString joinstyle;
    QString cap_style;
    QString outline_width;
    QString outline_width_unit;
    QString outline_width_map_unit_scale;
    QString color;
    QString outline_color;
}SMarkerSymbolEllipse;

///字符标记（Font marker）
typedef struct _SMarkerSymbolFont : public SMarkerSymbolBase{
    Q_GADGET
    Q_PROPERTY(QString font MEMBER font)
    Q_PROPERTY(QString chr MEMBER chr)
    Q_PROPERTY(QString color MEMBER color)
    Q_PROPERTY(QString angle MEMBER angle)
    Q_PROPERTY(QString font_style MEMBER font_style)
    Q_PROPERTY(QString outline_color MEMBER outline_color)
    Q_PROPERTY(QString outline_width MEMBER outline_width)
    Q_PROPERTY(QString outline_width_unit MEMBER outline_width_unit)
    Q_PROPERTY(QString outline_width_map_unit_scale MEMBER outline_width_map_unit_scale)
    Q_PROPERTY(QString joinstyle MEMBER joinstyle)

public:
    QString font = QStringLiteral("Dingbats"); //字体名称 Dingbats
    QString chr = QStringLiteral("%1").arg(QChar('A'));  //字符
    QString color = QStringLiteral("red");
    QString angle;
    QString font_style;
    QString outline_color;
    QString outline_width;
    QString outline_width_unit;
    QString outline_width_map_unit_scale;
    QString joinstyle;
}SMarkerSymbolFont;

///LLD_TODO Geometry Generator/Interpolated Line/
typedef struct _SLineSymbolBase {
    Q_GADGET
    Q_PROPERTY(QString offset MEMBER offset)
    Q_PROPERTY(QString offset_unit MEMBER offset_unit)
    Q_PROPERTY(QString offset_map_unit_scale MEMBER offset_map_unit_scale)
public:
    QString offset; //偏移量
    QString offset_unit; //偏移量单位 MM(毫米) 可取值如下 RenderMetersInMapUnits/meters/MapUnit/mapunits/Pixel/Percentage/percent/Point/points/Inch
    QString offset_map_unit_scale; //设置offset的最大最小scale 形式如下3x:%1,%2,%3,%4,%5,%6 见QgsSymbolLayerUtils::encodeMapUnitScale
}SLineSymbolBase;

///Simple line
typedef struct _SLineSymbolSimple : public SLineSymbolBase{
    Q_GADGET
    Q_PROPERTY(QString outline_color MEMBER outline_color);
    Q_PROPERTY(QString outline_width MEMBER outline_width);
    Q_PROPERTY(QString outline_style MEMBER outline_style);
    Q_PROPERTY(QString outline_width_unit MEMBER outline_width_unit);
    Q_PROPERTY(QString width_map_unit_scale MEMBER width_map_unit_scale);
    Q_PROPERTY(QString joinstyle MEMBER joinstyle);
    Q_PROPERTY(QString capstyle MEMBER capstyle);
    Q_PROPERTY(QString use_custom_dash MEMBER use_custom_dash);
    Q_PROPERTY(QString customdash MEMBER customdash);
    Q_PROPERTY(QString customdash_unit MEMBER customdash_unit);
    Q_PROPERTY(QString customdash_map_unit_scale MEMBER customdash_map_unit_scale);
    Q_PROPERTY(QString draw_inside_polygon MEMBER draw_inside_polygon);
    Q_PROPERTY(QString ring_filter MEMBER ring_filter);
    Q_PROPERTY(QString dash_pattern_offset MEMBER dash_pattern_offset);
    Q_PROPERTY(QString dash_pattern_offset_unit MEMBER dash_pattern_offset_unit);
    Q_PROPERTY(QString dash_pattern_offset_map_unit_scale MEMBER dash_pattern_offset_map_unit_scale);
    Q_PROPERTY(QString trim_distance_start MEMBER trim_distance_start);
    Q_PROPERTY(QString trim_distance_start_unit MEMBER trim_distance_start_unit);
    Q_PROPERTY(QString trim_distance_start_map_unit_scale MEMBER trim_distance_start_map_unit_scale);
    Q_PROPERTY(QString trim_distance_end MEMBER trim_distance_end);
    Q_PROPERTY(QString trim_distance_end_unit MEMBER trim_distance_end_unit);
    Q_PROPERTY(QString trim_distance_end_map_unit_scale MEMBER trim_distance_end_map_unit_scale);
    Q_PROPERTY(QString align_dash_pattern MEMBER align_dash_pattern);
    Q_PROPERTY(QString tweak_dash_pattern_on_corners MEMBER tweak_dash_pattern_on_corners);
public:
    QString outline_color = QStringLiteral("red");
    QString outline_width = QStringLiteral("0.26");
    QString outline_style = QStringLiteral("solid");
    QString outline_width_unit;
    QString width_map_unit_scale;
    QString joinstyle;
    QString capstyle;
    QString use_custom_dash;
    QString customdash;
    QString customdash_unit;
    QString customdash_map_unit_scale;
    QString draw_inside_polygon;
    QString ring_filter;
    QString dash_pattern_offset;
    QString dash_pattern_offset_unit;
    QString dash_pattern_offset_map_unit_scale;
    QString trim_distance_start;
    QString trim_distance_start_unit;
    QString trim_distance_start_map_unit_scale;
    QString trim_distance_end;
    QString trim_distance_end_unit;
    QString trim_distance_end_map_unit_scale;
    QString align_dash_pattern;
    QString tweak_dash_pattern_on_corners;
}SLineSymbolSimple;

///Raster Line
typedef struct _SLineSymbolRaster : public SLineSymbolBase{
    Q_GADGET
    Q_PROPERTY(QString line_width MEMBER line_width);
    Q_PROPERTY(QString line_width_unit MEMBER line_width_unit);
    Q_PROPERTY(QString width_map_unit_scale MEMBER width_map_unit_scale);
    Q_PROPERTY(QString imageFile MEMBER imageFile);
    Q_PROPERTY(QString joinstyle MEMBER joinstyle);
    Q_PROPERTY(QString capstyle MEMBER capstyle);
    Q_PROPERTY(QString alpha MEMBER alpha);
public:
    QString line_width = QStringLiteral("0.26");
    QString line_width_unit;
    QString width_map_unit_scale;
    QString imageFile;
    QString joinstyle;
    QString capstyle;
    QString alpha = QStringLiteral("1");;
}SLineSymbolRaster;

///标记线（Marker line）
typedef struct _SLineSymbolMarker : public SLineSymbolBase{
    Q_GADGET
    Q_PROPERTY(QString interval MEMBER interval);
    Q_PROPERTY(QString rotate MEMBER rotate);
    Q_PROPERTY(QString interval_unit MEMBER rotate);
    Q_PROPERTY(QString offset_along_line MEMBER rotate);
    Q_PROPERTY(QString offset_along_line_unit MEMBER rotate);
    Q_PROPERTY(QString offset_along_line_map_unit_scale MEMBER rotate);
    Q_PROPERTY(QString interval_map_unit_scale MEMBER rotate);
    Q_PROPERTY(QString average_angle_length MEMBER rotate);
    Q_PROPERTY(QString average_angle_unit MEMBER rotate);
    Q_PROPERTY(QString average_angle_map_unit_scale MEMBER rotate);
    Q_PROPERTY(QString placement MEMBER rotate);
    Q_PROPERTY(QString placements MEMBER rotate);
    Q_PROPERTY(QString ring_filter MEMBER rotate);
    Q_PROPERTY(QString place_on_every_part MEMBER rotate);

public:
    QString interval = QStringLiteral("3"); //
    QString rotate = QStringLiteral("1");   //0-false 1-true
    QString interval_unit;
    QString offset_along_line;
    QString offset_along_line_unit;
    QString offset_along_line_map_unit_scale;
    QString interval_map_unit_scale;
    QString average_angle_length;
    QString average_angle_unit;
    QString average_angle_map_unit_scale;
    QString placement;
    QString placements;
    QString ring_filter;
    QString place_on_every_part;
}SLineSymbolMarker;

///Hashed Line
typedef struct _SLineSymbolHashed : public SLineSymbolMarker{
    Q_GADGET
    Q_PROPERTY(QString hash_angle MEMBER hash_angle);
    Q_PROPERTY(QString hash_length MEMBER hash_length);
    Q_PROPERTY(QString hash_length_unit MEMBER hash_length_unit);
    Q_PROPERTY(QString hash_length_map_unit_scale MEMBER hash_length_map_unit_scale);
public:
    QString hash_angle;
    QString hash_length;
    QString hash_length_unit;
    QString hash_length_map_unit_scale;
}SLineSymbolHashed;

///Burst line
typedef struct _SLineSymbolBurst : public SLineSymbolBase{
    Q_GADGET
    Q_PROPERTY(QString line_width MEMBER line_width)
    Q_PROPERTY(QString line_width_unit MEMBER line_width_unit)
    Q_PROPERTY(QString width_map_unit_scale MEMBER width_map_unit_scale)
    Q_PROPERTY(QString joinstyle MEMBER joinstyle)
    Q_PROPERTY(QString capstyle MEMBER capstyle)
    Q_PROPERTY(QString color_type MEMBER color_type)
    Q_PROPERTY(QString color MEMBER color)
    Q_PROPERTY(QString gradient_color2 MEMBER gradient_color2)
    Q_PROPERTY(QString rampType MEMBER rampType)
public:
    QString line_width = QStringLiteral("0.26");
    QString line_width_unit;
    QString width_map_unit_scale;
    QString joinstyle;
    QString capstyle;
    QString color_type = QStringLiteral("0");//0-SimpleTwoColor 1-ColorRamp
    QString color  = QStringLiteral("black");
    QString gradient_color2 = QStringLiteral("white");
    QString rampType;
}SLineSymbolBurst;

///Arrow
typedef struct _SLineSymbolArrow : public SLineSymbolBase{
    Q_GADGET
    Q_PROPERTY(QString arrow_width MEMBER arrow_width)
    Q_PROPERTY(QString arrow_width_unit MEMBER arrow_width_unit)
    Q_PROPERTY(QString arrow_width_unit_scale MEMBER arrow_width_unit_scale)
    Q_PROPERTY(QString arrow_start_width MEMBER arrow_start_width)
    Q_PROPERTY(QString arrow_start_width_unit MEMBER arrow_start_width_unit)
    Q_PROPERTY(QString arrow_arrow_start_width_unit_scalewidth MEMBER arrow_arrow_start_width_unit_scalewidth)
    Q_PROPERTY(QString is_curved MEMBER is_curved)
    Q_PROPERTY(QString is_repeated MEMBER is_repeated)
    Q_PROPERTY(QString head_length MEMBER head_length)
    Q_PROPERTY(QString head_length_unit MEMBER head_length_unit)
    Q_PROPERTY(QString head_length_unit_scale MEMBER head_length_unit_scale)
    Q_PROPERTY(QString head_thickness MEMBER head_thickness)
    Q_PROPERTY(QString head_thickness_unit MEMBER head_thickness_unit)
    Q_PROPERTY(QString head_thickness_unit_scale MEMBER head_thickness_unit_scale)
    Q_PROPERTY(QString head_type MEMBER head_type)
    Q_PROPERTY(QString arrow_type MEMBER arrow_type)
    Q_PROPERTY(QString ring_filter MEMBER ring_filter)

public:
    QString arrow_width = QStringLiteral("1.0");
    QString arrow_width_unit;
    QString arrow_width_unit_scale;
    QString arrow_start_width= QStringLiteral("1.0");
    QString arrow_start_width_unit;
    QString arrow_arrow_start_width_unit_scalewidth;
    QString is_curved = QStringLiteral("1");
    QString is_repeated = QStringLiteral("1");;
    QString head_length= QStringLiteral("1.5");
    QString head_length_unit;
    QString head_length_unit_scale;
    QString head_thickness= QStringLiteral("1.5");
    QString head_thickness_unit;
    QString head_thickness_unit_scale;
    QString head_type = QStringLiteral("0");  //0-HeadSingle 1-HeadReversed 2-HeadDouble
    QString arrow_type = QStringLiteral("0"); //0-ArrowPlain 1-ArrowLeftHalf 2-ArrowRightHalf
    QString ring_filter;
}SLineSymbolArrow;

///LLD_TODO Geometry Generator
typedef struct _SFillSymbolBase {
    Q_GADGET
    Q_PROPERTY(QString offset MEMBER offset)
    Q_PROPERTY(QString offset_unit MEMBER offset_unit)
    Q_PROPERTY(QString offset_map_unit_scale MEMBER offset_map_unit_scale)
public:
    QString offset; //偏移量
    QString offset_unit; //偏移量单位 MM(毫米) 可取值如下 RenderMetersInMapUnits/meters/MapUnit/mapunits/Pixel/Percentage/percent/Point/points/Inch
    QString offset_map_unit_scale; //设置offset的最大最小scale 形式如下3x:%1,%2,%3,%4,%5,%6 见QgsSymbolLayerUtils::encodeMapUnitScale
}SFillSymbolBase;

/// 简单填充
typedef struct _SFillSymbolSimple : public SFillSymbolBase{
    Q_GADGET
    Q_PROPERTY(QString color MEMBER color)
    Q_PROPERTY(QString style MEMBER style)
    Q_PROPERTY(QString outline_color MEMBER outline_color)
    Q_PROPERTY(QString outline_style MEMBER outline_style)
    Q_PROPERTY(QString outline_width MEMBER outline_width)
    Q_PROPERTY(QString outline_width_unit MEMBER outline_width_unit)
    Q_PROPERTY(QString border_width_map_unit_scale MEMBER border_width_map_unit_scale)
    Q_PROPERTY(QString joinstyle MEMBER joinstyle)

public:
    QString color = QStringLiteral("red");  //颜色 颜色支持见 QgsSymbolLayerUtils::decodeColor
    QString style = QStringLiteral("solid");//
    QString outline_color = QStringLiteral("35, 35, 35"); //边框颜色
    QString outline_style = QStringLiteral("solid"); //线型
    QString outline_width = QStringLiteral("0.26"); //边框宽度
    QString outline_width_unit; //边框宽度单位    
    QString border_width_map_unit_scale;
    QString joinstyle = QStringLiteral("bevel");; //相交效果 QgsSymbolLayerUtils::decodePenJoinStyle
}SFillSymbolSimple;

///栅格数据填充（Raster image fill）
typedef struct _SFillSymbolRaster : public SFillSymbolBase{
    Q_GADGET
    Q_PROPERTY(QString imageFile MEMBER imageFile)
    Q_PROPERTY(QString coordinate_mode MEMBER coordinate_mode)
    Q_PROPERTY(QString alpha MEMBER alpha)
    Q_PROPERTY(QString angle MEMBER angle)
    Q_PROPERTY(QString width MEMBER width)
    Q_PROPERTY(QString width_unit MEMBER width_unit)
    Q_PROPERTY(QString width_map_unit_scale MEMBER width_map_unit_scale)
public:
    QString imageFile;          //文件路径
    QString coordinate_mode = QStringLiteral("0");    //0-Feature 1-Viewport
    QString alpha = QStringLiteral("1.0");            //0.0-1.0
    QString angle = QStringLiteral("0.0");
    QString width = QStringLiteral("0.0");
    QString width_unit;
    QString width_map_unit_scale;
}SFillSymbolRaster;

///SVG填充（SVG fill）
typedef struct _SFillSymbolSvg : public SFillSymbolBase{
    Q_GADGET
    Q_PROPERTY(QString svgFile MEMBER svgFile)
    Q_PROPERTY(QString data MEMBER data)
    Q_PROPERTY(QString width MEMBER width)
    Q_PROPERTY(QString angle MEMBER angle)
    Q_PROPERTY(QString color MEMBER color)
    Q_PROPERTY(QString outline_color MEMBER outline_color)
    Q_PROPERTY(QString outline_width MEMBER outline_width)
    Q_PROPERTY(QString pattern_width_unit MEMBER pattern_width_unit)
    Q_PROPERTY(QString pattern_width_map_unit_scale MEMBER pattern_width_map_unit_scale)
    Q_PROPERTY(QString svg_outline_width_unit MEMBER svg_outline_width_unit)
    Q_PROPERTY(QString svg_outline_width_map_unit_scale MEMBER svg_outline_width_map_unit_scale)
    Q_PROPERTY(QString outline_width_unit MEMBER outline_width_unit)
    Q_PROPERTY(QString outline_width_map_unit_scale MEMBER outline_width_map_unit_scale)
public:
    QString svgFile;
    QString data;
    QString width = QStringLiteral("12");
    QString angle = QStringLiteral("0");
    QString color = QStringLiteral("red");
    QString outline_color;
    QString outline_width;
    QString pattern_width_unit;
    QString pattern_width_map_unit_scale;
    QString svg_outline_width_unit;
    QString svg_outline_width_map_unit_scale;
    QString outline_width_unit;
    QString outline_width_map_unit_scale;
}SFillSymbolSvg;

///仅渲染中心点（Centroid fill）
typedef struct _SFillSymbolCentroid{
    Q_GADGET
    Q_PROPERTY(QString point_on_surface MEMBER point_on_surface)
    Q_PROPERTY(QString point_on_all_parts MEMBER point_on_all_parts)
    Q_PROPERTY(QString clip_points MEMBER clip_points)
    Q_PROPERTY(QString clip_on_current_part_only MEMBER clip_on_current_part_only)
public:
    QString point_on_surface;   //Force placement of markers inside polygons
    QString point_on_all_parts = QStringLiteral("1"); //Draw markers on every part of multi-part features
    QString clip_points;        //Clip markers to polygon boundary
    QString clip_on_current_part_only;  //Clip markers to current part boundary only
}SFillSymbolCentroid;

///渐变填充（Gradient fill）
typedef struct _SFillSymbolGradient : public SFillSymbolBase{
    Q_GADGET
    Q_PROPERTY(QString type MEMBER type)
    Q_PROPERTY(QString coordinate_mode MEMBER coordinate_mode)
    Q_PROPERTY(QString spread MEMBER spread)
    Q_PROPERTY(QString color_type MEMBER color_type)
    Q_PROPERTY(QString color MEMBER color)
    Q_PROPERTY(QString gradient_color2 MEMBER gradient_color2)
    Q_PROPERTY(QString reference_point1 MEMBER reference_point1)
    Q_PROPERTY(QString reference_point1_iscentroid MEMBER reference_point1_iscentroid)
    Q_PROPERTY(QString reference_point2 MEMBER reference_point2)
    Q_PROPERTY(QString reference_point2_iscentroid MEMBER reference_point2_iscentroid)
    Q_PROPERTY(QString angle MEMBER angle)
    Q_PROPERTY(QString rampType MEMBER rampType)

public:
    QString type = QStringLiteral("0");   //0-Linear/1-Radial/2-Conical
    QString coordinate_mode = QStringLiteral("0");    //0-Feature 1-Viewport
    QString spread = QStringLiteral("0"); //0-Pad 1-Reflect 2-Repeat
    QString color_type = QStringLiteral("0");//0-SimpleTwoColor 1-ColorRamp
    QString color = QStringLiteral("red");//
    QString gradient_color2 = QStringLiteral("yellow");//
    QString reference_point1 = QStringLiteral("0,0");// x,y
    QString reference_point1_iscentroid; //
    QString reference_point2 = QStringLiteral("1,0");// x,y
    QString reference_point2_iscentroid;//三
    QString angle = QStringLiteral("0");  //
    QString rampType;//cpt-city/gradient
}SFillSymbolGradient;

///线符号填充（Line pattern fill）
typedef struct _SFillSymbolLinePattern : public SFillSymbolBase{
    Q_GADGET
    Q_PROPERTY(QString angle MEMBER angle)
    Q_PROPERTY(QString distance MEMBER distance)
    Q_PROPERTY(QString outline_width MEMBER outline_width)
    Q_PROPERTY(QString outline_color MEMBER outline_color)
    Q_PROPERTY(QString distance_unit MEMBER distance_unit)
    Q_PROPERTY(QString distance_map_unit_scale MEMBER distance_map_unit_scale)
    Q_PROPERTY(QString outline_width_unit MEMBER outline_width_unit)
    Q_PROPERTY(QString outline_width_map_unit_scale MEMBER outline_width_map_unit_scale)
    Q_PROPERTY(QString coordinate_reference MEMBER coordinate_reference)
    Q_PROPERTY(QString clip_mode MEMBER clip_mode)
public:
    QString angle = QStringLiteral("45");
    QString distance = QStringLiteral("5");
    QString outline_width = QStringLiteral("0.5");
    QString outline_color = QStringLiteral("black");
    QString distance_unit;
    QString distance_map_unit_scale;
    QString outline_width_unit;
    QString outline_width_map_unit_scale;
    QString coordinate_reference;
    QString clip_mode;
}SFillSymbolLinePattern;

///点符号填充(Point pattern fill)
typedef struct _SFillSymbolPointPattern{
    Q_GADGET
    Q_PROPERTY(QString distance_x MEMBER distance_x)
    Q_PROPERTY(QString distance_y MEMBER distance_y)
    Q_PROPERTY(QString displacement_x MEMBER displacement_x)
    Q_PROPERTY(QString displacement_y MEMBER displacement_y)
    Q_PROPERTY(QString offset_x MEMBER offset_x)
    Q_PROPERTY(QString offset_y MEMBER offset_y)
    Q_PROPERTY(QString distance_x_unit MEMBER distance_x_unit)
    Q_PROPERTY(QString distance_x_map_unit_scale MEMBER distance_x_map_unit_scale)
    Q_PROPERTY(QString distance_y_unit MEMBER distance_y_unit)
    Q_PROPERTY(QString distance_y_map_unit_scale MEMBER distance_y_map_unit_scale)
    Q_PROPERTY(QString displacement_x_unit MEMBER displacement_x_unit)
    Q_PROPERTY(QString displacement_x_map_unit_scale MEMBER displacement_x_map_unit_scale)
    Q_PROPERTY(QString displacement_y_unit MEMBER displacement_y_unit)
    Q_PROPERTY(QString displacement_y_map_unit_scale MEMBER displacement_y_map_unit_scale)
    Q_PROPERTY(QString offset_x_unit MEMBER offset_x_unit)
    Q_PROPERTY(QString offset_x_map_unit_scale MEMBER offset_x_map_unit_scale)
    Q_PROPERTY(QString offset_y_unit MEMBER offset_y_unit)
    Q_PROPERTY(QString offset_y_map_unit_scale MEMBER offset_y_map_unit_scale)
    Q_PROPERTY(QString random_deviation_x MEMBER random_deviation_x)
    Q_PROPERTY(QString random_deviation_y MEMBER random_deviation_y)
    Q_PROPERTY(QString random_deviation_x_unit MEMBER random_deviation_x_unit)
    Q_PROPERTY(QString random_deviation_x_map_unit_scale MEMBER random_deviation_x_map_unit_scale)
    Q_PROPERTY(QString random_deviation_y_unit MEMBER random_deviation_y_unit)
    Q_PROPERTY(QString random_deviation_y_map_unit_scale MEMBER random_deviation_y_map_unit_scale)
    Q_PROPERTY(QString seed MEMBER seed)
    Q_PROPERTY(QString outline_width_unit MEMBER outline_width_unit)
    Q_PROPERTY(QString outline_width_map_unit_scale MEMBER outline_width_map_unit_scale)
    Q_PROPERTY(QString clip_mode MEMBER clip_mode)
    Q_PROPERTY(QString coordinate_reference MEMBER coordinate_reference)
    Q_PROPERTY(QString angle MEMBER angle)
public:
    QString distance_x = QStringLiteral("15");
    QString distance_y = QStringLiteral("15");
    QString displacement_x = QStringLiteral("0");
    QString displacement_y = QStringLiteral("0");
    QString offset_x = QStringLiteral("0");
    QString offset_y = QStringLiteral("0");
    QString distance_x_unit;
    QString distance_x_map_unit_scale;
    QString distance_y_unit;
    QString distance_y_map_unit_scale;
    QString displacement_x_unit;
    QString displacement_x_map_unit_scale;
    QString displacement_y_unit;
    QString displacement_y_map_unit_scale;
    QString offset_x_unit;
    QString offset_x_map_unit_scale;
    QString offset_y_unit;
    QString offset_y_map_unit_scale;
    QString random_deviation_x = QStringLiteral("0");
    QString random_deviation_y = QStringLiteral("0");
    QString random_deviation_x_unit;
    QString random_deviation_x_map_unit_scale;
    QString random_deviation_y_unit;
    QString random_deviation_y_map_unit_scale;
    QString seed;
    QString outline_width_unit;
    QString outline_width_map_unit_scale;
    QString clip_mode = QStringLiteral("shape");  //no/shape/centroid_within/completely_within
    QString coordinate_reference = QStringLiteral("feature");   //feature/viewport
    QString angle;
}SFillSymbolPointPattern;

///随机Marker填充(Random Marker fill)
typedef struct _SFillSymbolRandomMarker{
    Q_GADGET
    Q_PROPERTY(QString count_method MEMBER count_method)
    Q_PROPERTY(QString point_count MEMBER point_count)
    Q_PROPERTY(QString density_area MEMBER density_area)
    Q_PROPERTY(QString seed MEMBER seed)
    Q_PROPERTY(QString density_area_unit MEMBER density_area_unit)
    Q_PROPERTY(QString density_area_unit_scale MEMBER density_area_unit_scale)
    Q_PROPERTY(QString clip_points MEMBER clip_points)
public:
    QString count_method = QStringLiteral("0"); //0-Absolute 1-DensityBased
    QString point_count  = QStringLiteral("10");
    QString density_area = QStringLiteral("250.0");
    QString seed;
    QString density_area_unit;
    QString density_area_unit_scale;
    QString clip_points;
}SFillSymbolRandomMarker;

///形状炸裂填充（Shapeburst fill）
typedef struct _SFillSymbolShapeburst{
    Q_GADGET
    Q_PROPERTY(QString color_type MEMBER color_type)
    Q_PROPERTY(QString color MEMBER color)
    Q_PROPERTY(QString gradient_color2 MEMBER gradient_color2)
    Q_PROPERTY(QString blur_radius MEMBER blur_radius)
    Q_PROPERTY(QString use_whole_shape MEMBER use_whole_shape)
    Q_PROPERTY(QString max_distance MEMBER max_distance)
    Q_PROPERTY(QString rampType MEMBER rampType)
    Q_PROPERTY(QString distance_unit MEMBER distance_unit)
    Q_PROPERTY(QString distance_map_unit_scale MEMBER distance_map_unit_scale)
    Q_PROPERTY(QString ignore_rings MEMBER ignore_rings)
public:
    QString color_type = QStringLiteral("0");//0-SimpleTwoColor 1-ColorRamp
    QString color  = QStringLiteral("red");
    QString gradient_color2 = QStringLiteral("white");
    QString blur_radius = QStringLiteral("0");
    QString use_whole_shape = QStringLiteral("1"); //0-false 1-true
    QString max_distance = QStringLiteral("5");
    QString rampType;
    QString distance_unit;
    QString distance_map_unit_scale;
    QString ignore_rings;
}SFillSymbolShapeburst;



#endif // LDCOMMON_H
