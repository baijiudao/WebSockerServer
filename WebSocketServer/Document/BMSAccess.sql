CREATE TABLE [dbo].[BMS_Alarm] (
	[Id] [int] IDENTITY (1, 1) NOT NULL ,
	[AlarmID] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmCategory] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmType] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmPriority] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmGlobalID] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmName] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmValue] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmTime] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmLimit] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmMessage] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmSource] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[ActorId] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmAck] [varchar] (8) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmStatus] [varchar] (8) COLLATE Chinese_PRC_CI_AS NULL ,
	[Reserve] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmChangeMask] [varchar] (16) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmNewState] [varchar] (16) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmQuality] [varchar] (16) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmActiveTime] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmConditionName] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[AlarmSubConditionName] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BMS_Device] (
	[Id] [int] IDENTITY (1, 1) NOT NULL ,
	[DeviceID] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[DeviceName] [varchar] (64) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BMS_Event] (
	[Id] [int] IDENTITY (1, 1) NOT NULL ,
	[EventID] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[EventCategory] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[EventType] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[UserName] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[AccessLevel] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[EventTime] [varchar] (32) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[EventSource] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[EventRecord] [varchar] (64) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BMS_HdsDevPrcs] (
	[ID] [int] IDENTITY (1, 1) NOT NULL ,
	[TagId] [varchar] (32) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[IOGlobalID] [varchar] (32) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[StartTime] [varchar] (32) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[EndTime] [varchar] (32) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[AvgValue] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[MaxValue] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[MaxValTime] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[MinValue] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[MinValTime] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[RunningTime] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[StateDesc] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BMS_HdsPrcs] (
	[ID] [int] IDENTITY (1, 1) NOT NULL ,
	[TagId] [varchar] (32) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[IOGlobalID] [varchar] (32) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[StartTime] [varchar] (32) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[EndTime] [varchar] (32) COLLATE Chinese_PRC_CI_AS NOT NULL ,
	[AvgValue] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[MaxValue] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[MaxValTime] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[MinValue] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[MinValTime] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BMS_SubSystem] (
	[Id] [int] IDENTITY (1, 1) NOT NULL ,
	[SubID] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[SubName] [varchar] (64) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BMS_Tag] (
	[Id] [int] IDENTITY (1, 1) NOT NULL ,
	[iTagId][int] NOT NULL,
	[TagID] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[TagName] [varchar] (64) COLLATE Chinese_PRC_CI_AS NULL ,
	[TagType] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[TagEngUnit] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[TagComment] [varchar] (64) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BMS历史数据表] (
	[BMS采集变量编号] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[采集值] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[采集时间] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[供应商] (
	[供应商代码] [varchar] (10) COLLATE Chinese_PRC_CI_AS NULL ,
	[供应商名称] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[法人代表] [varchar] (20) COLLATE Chinese_PRC_CI_AS NULL ,
	[联系部门] [varchar] (30) COLLATE Chinese_PRC_CI_AS NULL ,
	[联系人] [varchar] (20) COLLATE Chinese_PRC_CI_AS NULL ,
	[联系电话] [varchar] (15) COLLATE Chinese_PRC_CI_AS NULL ,
	[传真] [varchar] (15) COLLATE Chinese_PRC_CI_AS NULL ,
	[地址] [varchar] (60) COLLATE Chinese_PRC_CI_AS NULL ,
	[邮政编码] [varchar] (6) COLLATE Chinese_PRC_CI_AS NULL ,
	[电子邮箱] [varchar] (50) COLLATE Chinese_PRC_CI_AS NULL ,
	[网址] [varchar] (60) COLLATE Chinese_PRC_CI_AS NULL ,
	[备注] [varchar] (100) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[功能类别表] (
	[类编号] [int] NULL ,
	[类名] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[存档文件名] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[帮助文件名] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[备件入库记录] (
	[入库编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[备件编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[入库日期] [smalldatetime] NULL ,
	[采购员] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[保管员] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[验收人] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[验收结果] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL ,
	[备注] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[备件出库记录] (
	[出库编号] [varchar] (10) COLLATE Chinese_PRC_CI_AS NULL ,
	[备件编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[领料单位] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[领料人] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[出库日期] [smalldatetime] NULL ,
	[保管员] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[备注] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[备件类别] (
	[备件类别] [varchar] (24) COLLATE Chinese_PRC_CI_AS NULL ,
	[说明] [varchar] (100) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[备品备件] (
	[备件编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[备件类别] [varchar] (24) COLLATE Chinese_PRC_CI_AS NULL ,
	[供应商代码] [varchar] (10) COLLATE Chinese_PRC_CI_AS NULL ,
	[备件名称] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[备件型号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[库存数量] [real] NULL ,
	[计量单位] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[存放位置] [varchar] (24) COLLATE Chinese_PRC_CI_AS NULL ,
	[库存下限] [real] NULL ,
	[库存上限] [real] NULL ,
	[上次入库日期] [smalldatetime] NULL ,
	[上次入库数量] [real] NULL ,
	[入库总数量] [real] NULL ,
	[生产厂家] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[用途] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL ,
	[选项1] [varchar] (20) COLLATE Chinese_PRC_CI_AS NULL ,
	[选项2] [varchar] (20) COLLATE Chinese_PRC_CI_AS NULL ,
	[备注] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[故障类别] (
	[故障类别] [varchar] (20) COLLATE Chinese_PRC_CI_AS NULL ,
	[类别说明] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL ,
	[故障特征] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[耗能类别] (
	[耗能类别] [varchar] (30) COLLATE Chinese_PRC_CI_AS NULL ,
	[类别说明] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[能源数据采集配置表] (
	[计量表编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[采集变量编号] [varchar] (32) COLLATE Chinese_PRC_CI_AS NULL ,
	[设备编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[序号] [int] NULL ,
	[说明] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[计量表] (
	[计量表编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[计量表类型] [varchar] (24) COLLATE Chinese_PRC_CI_AS NULL ,
	[耗能类别] [varchar] (30) COLLATE Chinese_PRC_CI_AS NULL ,
	[表型号] [varchar] (20) COLLATE Chinese_PRC_CI_AS NULL ,
	[安装位置] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[安装日期] [smalldatetime] NULL ,
	[生产厂家] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[表倍率] [smallint] NULL ,
	[采集参数序号] [int],
	[转表读数] [real] NULL ,
	[转表基数] [real] NULL ,
	[标准计量单位] [varchar] (8) COLLATE Chinese_PRC_CI_AS NULL ,
	[费率标准] [real] NULL ,
	[使用单位] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[表的初始读数] [real] NULL ,
	[备注] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[计量表类型] (
	[计量表类型] [varchar] (24) COLLATE Chinese_PRC_CI_AS NULL ,
	[类型说明] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[设备] (
	[设备编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[供应商代码] [varchar] (10) COLLATE Chinese_PRC_CI_AS NULL ,
	[部件编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[参数序号] [int] NULL ,
	[设备名称] [varchar] (24) COLLATE Chinese_PRC_CI_AS NULL ,
	[安装位置] [varchar] (60) COLLATE Chinese_PRC_CI_AS NULL ,
	[出厂序号] [varchar] (24) COLLATE Chinese_PRC_CI_AS NULL ,
	[生产日期] [smalldatetime] NULL ,
	[购买日期] [smalldatetime] NULL ,
	[安装日期] [smalldatetime] NULL ,
	[使用日期] [smalldatetime] NULL ,
	[设备状况] [varchar] (50) COLLATE Chinese_PRC_CI_AS NULL ,
	[交接验收日期] [smalldatetime] NULL ,
	[设备供应商] [varchar] (60) COLLATE Chinese_PRC_CI_AS NULL ,
	[使用部门] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[备注] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL ,
	[报警上限] [real] NULL ,
	[报警下限] [real] NULL ,
	[运行控制上限] [real] NULL ,
	[运行控制下限] [real] NULL ,
	[运行参数记录表] [varchar] (20) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[设备停用记录] (
	[设备编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[停用编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[停用类型] [varchar] (24) COLLATE Chinese_PRC_CI_AS NULL ,
	[停用原因] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL ,
	[开始时间] [smalldatetime] NULL ,
	[结束时间] [smalldatetime] NULL ,
	[记录员] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[备注] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[设备参数表] (
	[设备名称] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[参数序号] [int],
	[参数名称] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[参数单位] [varchar] (10) COLLATE Chinese_PRC_CI_AS NULL ,
	[参数说明] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[设备故障] (
	[设备编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[故障编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[故障类别] [varchar] (10) COLLATE Chinese_PRC_CI_AS NULL ,
	[故障原因] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL ,
	[具体内容] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL ,
	[发生时间] [smalldatetime] NULL ,
	[排除时间] [smalldatetime] NULL ,
	[故障位置] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[排除结果] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL ,
	[值班员] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[备注] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[设备维修记录] (
	[记录id][int] IDENTITY (1, 1),
	[设备名称] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[维修内容] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[维修时间] [smalldatetime] NULL ,
	[维修单位] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[故障原因] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL ,
	[维修结果] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL ,
	[维修经办人] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[备注] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[设备运行时间] (
	[设备编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[记录时间] [smalldatetime] NULL ,
	[记录员] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[开机时间] [smalldatetime] NULL ,
	[关机时间] [smalldatetime] NULL ,
	[设备状态] [varchar] (24) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[设备运行记录] (
	[记录id][int] IDENTITY (1, 1),
	[设备名称] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[开始时间] [smalldatetime] NULL ,
	[结束时间] [smalldatetime] NULL ,
	[运行情况] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL ,
	[记录员] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[备注] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[设备部件] (
	[设备编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[部件编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[部件名称] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[部件型号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[可使用年限] [real] NULL ,
	[已使用年限] [real] NULL ,
	[部件数量] [real] NULL ,
	[部件说明] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[设备部件更换记录] (
	[设备编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[更换编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[备件编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[更换日期] [smalldatetime] NULL ,
	[预计更换残值] [real] NULL ,
	[更换经办人] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[备注] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[走表读数] (
	[计量表编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[读表日期] [smalldatetime] NULL ,
	[上次走表读数] [real] NULL ,
	[走表读数] [real] NULL ,
	[费率(元)] [real] NULL ,
	[记录员] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[转表或换表标志] [varchar] (4) COLLATE Chinese_PRC_CI_AS NULL ,
	[旧表终止读数] [real] NULL ,
	[新表起始读数] [real] NULL ,
	[备注] [varchar] (100) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[附属设备] (
	[设备编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[附属编号] [varchar] (12) COLLATE Chinese_PRC_CI_AS NULL ,
	[设备名称] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[设备说明] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL ,
	[额定电压] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[额定电流] [nvarchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[额定功率] [varchar] (40) COLLATE Chinese_PRC_CI_AS NULL ,
	[额定频率] [varchar] (60) COLLATE Chinese_PRC_CI_AS NULL ,
	[额定转速] [varchar] (60) COLLATE Chinese_PRC_CI_AS NULL ,
	[生产日期] [smalldatetime] NULL ,
	[出厂序号] [varchar] (24) COLLATE Chinese_PRC_CI_AS NULL ,
	[使用日期] [smalldatetime] NULL ,
	[使用年限] [varchar] (8) COLLATE Chinese_PRC_CI_AS NULL ,
	[生产厂家] [varchar] (80) COLLATE Chinese_PRC_CI_AS NULL ,
	[选项1] [varchar] (20) COLLATE Chinese_PRC_CI_AS NULL ,
	[选项2] [varchar] (20) COLLATE Chinese_PRC_CI_AS NULL ,
	[备注] [varchar] (200) COLLATE Chinese_PRC_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[BMS_IOValue] (
	[Id] [int] IDENTITY (1, 1),
	[IOGlobalId] [int] NOT NULL ,
	[TagType] [int] NULL,
	[IOValue] [varchar] (32) NOT NULL ,
	[IOTime] [datetime] NOT NULL ,
	[Quality] [int] NULL ,
	CONSTRAINT [PK_BMS_IOValue] PRIMARY KEY  CLUSTERED(Id,IOTime)
)
ON [PRIMARY]
GO
CREATE TABLE [dbo].[BMS_IOHistroyValue] (
	[Id] [int] IDENTITY (1, 1),
	[IOGlobalId] [int] NOT NULL ,
	[TagType] [int] NULL,
	[IOValue] [varchar] (32) NOT NULL ,
	[IOTime] [datetime] NOT NULL ,
	[Quality] [int] NULL ,
	CONSTRAINT [PK_BMS_IOHistroyValue] PRIMARY KEY  CLUSTERED(Id,IOTime)
)
ON [PRIMARY]
GO

ALTER TABLE [dbo].[BMS_Alarm] WITH NOCHECK ADD 
	CONSTRAINT [PK_BMS_Alarm] PRIMARY KEY  CLUSTERED 
	(
		[Id]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BMS_Device] WITH NOCHECK ADD 
	CONSTRAINT [PK_BMS_Device] PRIMARY KEY  CLUSTERED 
	(
		[Id]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BMS_Event] WITH NOCHECK ADD 
	CONSTRAINT [PK_BMS_Event] PRIMARY KEY  CLUSTERED 
	(
		[Id]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BMS_HdsDevPrcs] WITH NOCHECK ADD 
	CONSTRAINT [PK_BMS_HdsDevPrcs] PRIMARY KEY  CLUSTERED 
	(
		[ID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BMS_HdsPrcs] WITH NOCHECK ADD 
	CONSTRAINT [PK_BMS_HdsPrcs] PRIMARY KEY  CLUSTERED 
	(
		[ID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BMS_SubSystem] WITH NOCHECK ADD 
	CONSTRAINT [PK_BMS_SubSystem] PRIMARY KEY  CLUSTERED 
	(
		[Id]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[BMS_Tag] WITH NOCHECK ADD 
	CONSTRAINT [PK_BMS_Tag] PRIMARY KEY  CLUSTERED 
	(
		[Id]
	)  ON [PRIMARY] 
GO

 CREATE  INDEX [IX_BMS_Alarm] ON [dbo].[BMS_Alarm]([AlarmTime]) ON [PRIMARY]
GO

 CREATE  INDEX [IX_BMS_Event] ON [dbo].[BMS_Event]([EventTime]) ON [PRIMARY]
GO

 CREATE  INDEX [IX_BMS_HdsDevPrcs] ON [dbo].[BMS_HdsDevPrcs]([TagId], [StartTime], [EndTime]) ON [PRIMARY]
GO

 CREATE  INDEX [IX_BMS_HdsPrcs] ON [dbo].[BMS_HdsPrcs]([TagId], [StartTime], [EndTime]) ON [PRIMARY]
GO
