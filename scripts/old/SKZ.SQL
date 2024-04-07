if not exists (select * from master.dbo.syslogins where loginname = N'Cinema')
BEGIN
	declare @logindb nvarchar(132), @loginlang nvarchar(132) select @logindb = N'CinemaSec', @loginlang = N'us_english'
	if @logindb is null or not exists (select * from master.dbo.sysdatabases where name = @logindb)
		select @logindb = N'master'
	if @loginlang is null or (not exists (select * from master.dbo.syslanguages where name = @loginlang) and @loginlang <> N'us_english')
		select @loginlang = @@language
	exec sp_addlogin N'Cinema', N'Cinema', @logindb, @loginlang
END
GO 



if not exists (select * from dbo.sysusers where name = N'Cinema' and uid < 16382)
	EXEC sp_grantdbaccess N'Cinema', N'Cinema'
GO

exec sp_addrolemember N'db_accessadmin', N'Cinema'
GO

exec sp_addrolemember N'db_backupoperator', N'Cinema'
GO

exec sp_addrolemember N'db_datareader', N'Cinema'
GO

exec sp_addrolemember N'db_datawriter', N'Cinema'
GO

exec sp_addrolemember N'db_ddladmin', N'Cinema'
GO

exec sp_addrolemember N'db_owner', N'Cinema'
GO

exec sp_addrolemember N'db_securityadmin', N'Cinema'
GO

CREATE TABLE [dbo].[Cinemas] (
	[CinemaID] [int] IDENTITY (1, 1) NOT NULL ,
	[CinemaName] [nvarchar] (50) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL ,
	[CinemaComments] [nvarchar] (50) COLLATE SQL_Latin1_General_CP1_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[Rooms] (
	[RoomID] [int] IDENTITY (1, 1) NOT NULL ,
	[CinemaID] [int] NOT NULL ,
	[RoomNumber] [int] NULL ,
	[RoomChairs] [image] NULL ,
	[RoomIP] [varchar] (50) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL ,
	[AlgoParams] [image] NULL ,
	[TimeAfterStart] [int] NOT NULL ,
	[TimeBeforeEnd] [int] NOT NULL ,
	[TimePeriod] [int] NOT NULL ,
	[BackupStart] [decimal](6, 2) NOT NULL ,
	[BackupEnd] [decimal](6, 2) NOT NULL ,
	[BackupDate] [datetime] NOT NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO

CREATE TABLE [dbo].[Cameras] (
	[CameraID] [int] IDENTITY (1, 1) NOT NULL ,
	[RoomID] [int] NOT NULL ,
	[CameraType] [smallint] NOT NULL ,
	[CameraCoord] [int] NOT NULL ,
	[CameraUID] [varchar] (80) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL ,
	[CameraIP] [varchar] (50) COLLATE SQL_Latin1_General_CP1_CI_AS NULL ,
	[CameraBackground] [image] NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO

CREATE TABLE [dbo].[Protocol] (
	[RoomID] [int] NOT NULL ,
	[ProtoTime] [datetime] NOT NULL ,
	[ProtoType] [smallint] NOT NULL ,
	[ProtoString] [text] COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO

CREATE TABLE [dbo].[Statistics] (
	[StatID] [int] IDENTITY (1, 1) NOT NULL ,
	[RoomID] [int] NOT NULL ,
	[StatTime] [datetime] NOT NULL ,
	[StatViewerNumber] [smallint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[Timetable] (
	[FilmID] [int] IDENTITY (1, 1) NOT NULL ,
	[RoomID] [int] NOT NULL ,
	[Name] [nvarchar] (500) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL ,
	[BeginTime] [datetime] NOT NULL ,
	[EndTime] [datetime] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[StatPhotos] (
	[StatID] [int] NOT NULL ,
	[CameraID] [int] NOT NULL ,
	[StatPhoto] [image] NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO

ALTER TABLE [dbo].[Cinemas] WITH NOCHECK ADD 
	CONSTRAINT [PK_Cinemas] PRIMARY KEY  CLUSTERED 
	(
		[CinemaID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[Rooms] WITH NOCHECK ADD 
	CONSTRAINT [PK_Rooms] PRIMARY KEY  CLUSTERED 
	(
		[RoomID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[Cameras] WITH NOCHECK ADD 
	CONSTRAINT [PK_Cameras] PRIMARY KEY  CLUSTERED 
	(
		[CameraID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[Statistics] WITH NOCHECK ADD 
	CONSTRAINT [PK_Statistics] PRIMARY KEY  CLUSTERED 
	(
		[StatID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[Timetable] WITH NOCHECK ADD 
	CONSTRAINT [PK_Timetable] PRIMARY KEY  CLUSTERED 
	(
		[FilmID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[StatPhotos] WITH NOCHECK ADD 
	CONSTRAINT [PK_StatPhotos] PRIMARY KEY  CLUSTERED 
	(
		[StatID],
		[CameraID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[Rooms] WITH NOCHECK ADD 
	CONSTRAINT [DF_Rooms_RoomChairs] DEFAULT (null) FOR [RoomChairs],
	CONSTRAINT [DF_Rooms_AlgoParams] DEFAULT (null) FOR [AlgoParams],
	CONSTRAINT [DF_Rooms_TimeAfterStart] DEFAULT (10) FOR [TimeAfterStart],
	CONSTRAINT [DF_Rooms_TimeBeforeEnd] DEFAULT (10) FOR [TimeBeforeEnd],
	CONSTRAINT [DF_Rooms_TimePeriod] DEFAULT (10) FOR [TimePeriod],
	CONSTRAINT [DF_Rooms_BackupStart] DEFAULT (20.00) FOR [BackupStart],
	CONSTRAINT [DF_Rooms_BackupEnd] DEFAULT (50.00) FOR [BackupEnd],
	CONSTRAINT [DF_Rooms_BackupDate] DEFAULT ('2005-02-20 00:00:00') FOR [BackupDate],
	CONSTRAINT [CK_Rooms] CHECK ([BackupStart] >= 0 and [BackupStart] < 100 and [BackupEnd] >= 0 and [BackupEnd] < 100 and [BackupStart] <= [BackupEnd])
GO

ALTER TABLE [dbo].[Rooms] ADD 
	CONSTRAINT [FK_Rooms_Cinemas] FOREIGN KEY 
	(
		[CinemaID]
	) REFERENCES [dbo].[Cinemas] (
		[CinemaID]
	) ON DELETE CASCADE  ON UPDATE CASCADE 
GO

ALTER TABLE [dbo].[Cameras] ADD 
	CONSTRAINT [FK_Cameras_Rooms] FOREIGN KEY 
	(
		[RoomID]
	) REFERENCES [dbo].[Rooms] (
		[RoomID]
	) ON DELETE CASCADE  ON UPDATE CASCADE 
GO

ALTER TABLE [dbo].[Protocol] ADD 
	CONSTRAINT [FK_Protocol_Rooms] FOREIGN KEY 
	(
		[RoomID]
	) REFERENCES [dbo].[Rooms] (
		[RoomID]
	) ON DELETE CASCADE  ON UPDATE CASCADE 
GO

ALTER TABLE [dbo].[Statistics] ADD 
	CONSTRAINT [FK_Statistics_Rooms] FOREIGN KEY 
	(
		[RoomID]
	) REFERENCES [dbo].[Rooms] (
		[RoomID]
	) ON DELETE CASCADE  ON UPDATE CASCADE 
GO

ALTER TABLE [dbo].[Timetable] ADD 
	CONSTRAINT [FK_Timetable_Rooms] FOREIGN KEY 
	(
		[RoomID]
	) REFERENCES [dbo].[Rooms] (
		[RoomID]
	) ON DELETE CASCADE  ON UPDATE CASCADE 
GO

ALTER TABLE [dbo].[StatPhotos] ADD 
	CONSTRAINT [FK_StatPhotos_Cameras] FOREIGN KEY 
	(
		[CameraID]
	) REFERENCES [dbo].[Cameras] (
		[CameraID]
	),
	CONSTRAINT [FK_StatPhotos_Statistics] FOREIGN KEY 
	(
		[StatID]
	) REFERENCES [dbo].[Statistics] (
		[StatID]
	) ON DELETE CASCADE  ON UPDATE CASCADE 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO


CREATE  TRIGGER TimePeriods
ON dbo.Timetable
FOR INSERT, UPDATE 
AS
DECLARE @BeginTime datetime, @EndTime datetime
SELECT @BeginTime = BeginTime, 
   @EndTime = EndTime 
FROM timetable
IF @BeginTime >= @EndTime
BEGIN
   RAISERROR ('Left bound of a time period must be greater than right one', 16, 1)
   ROLLBACK TRANSACTION
END

DECLARE @BeginTimePrev datetime, @EndTimePrev datetime,
	@BeginTimeNext datetime, @EndTimeNext datetime,
	@RoomIDPrev int, @RoomIDNext int


DECLARE TimeCursor CURSOR
FOR 	SELECT BeginTime, EndTime, RoomID FROM Timetable
	ORDER BY RoomID ASC, BeginTime ASC

OPEN TimeCursor

FETCH NEXT FROM TimeCursor 
INTO @BeginTimePrev, @EndTimePrev, @RoomIDPrev

WHILE @@FETCH_STATUS = 0
BEGIN

FETCH NEXT FROM TimeCursor INTO @BeginTimeNext, @EndTimeNext, @RoomIDNext

IF @@FETCH_STATUS = 0
BEGIN
	IF @BeginTimeNext <= @EndTimePrev AND @RoomIDNext = @RoomIDPrev
	BEGIN
	RAISERROR ('Some time periods intersect', 16, 1)
	ROLLBACK TRANSACTION
	BREAK
	END
SET @BeginTimePrev = @BeginTimeNext
SET @EndTimePrev = @EndTimeNext
END

END

CLOSE TimeCursor
DEALLOCATE TimeCursor


GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

