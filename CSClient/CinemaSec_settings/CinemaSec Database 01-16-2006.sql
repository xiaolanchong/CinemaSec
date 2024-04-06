IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'CinemaSec')
	DROP DATABASE [CinemaSec]
GO

CREATE DATABASE [CinemaSec]  ON (NAME = N'CinemaSec', FILENAME = N'E:\CinemaSec\_Archive\CinemaSec.mdf' , SIZE = 1731, FILEGROWTH = 10%) LOG ON (NAME = N'SKZ_log', FILENAME = N'E:\CinemaSec\_Archive\CinemaSec.mdf.LDF' , SIZE = 6, FILEGROWTH = 10%)
 COLLATE Cyrillic_General_CI_AS
GO

exec sp_dboption N'CinemaSec', N'autoclose', N'false'
GO

exec sp_dboption N'CinemaSec', N'bulkcopy', N'false'
GO

exec sp_dboption N'CinemaSec', N'trunc. log', N'false'
GO

exec sp_dboption N'CinemaSec', N'torn page detection', N'true'
GO

exec sp_dboption N'CinemaSec', N'read only', N'false'
GO

exec sp_dboption N'CinemaSec', N'dbo use', N'false'
GO

exec sp_dboption N'CinemaSec', N'single', N'false'
GO

exec sp_dboption N'CinemaSec', N'autoshrink', N'false'
GO

exec sp_dboption N'CinemaSec', N'ANSI null default', N'false'
GO

exec sp_dboption N'CinemaSec', N'recursive triggers', N'false'
GO

exec sp_dboption N'CinemaSec', N'ANSI nulls', N'false'
GO

exec sp_dboption N'CinemaSec', N'concat null yields null', N'false'
GO

exec sp_dboption N'CinemaSec', N'cursor close on commit', N'false'
GO

exec sp_dboption N'CinemaSec', N'default to local cursor', N'false'
GO

exec sp_dboption N'CinemaSec', N'quoted identifier', N'false'
GO

exec sp_dboption N'CinemaSec', N'ANSI warnings', N'false'
GO

exec sp_dboption N'CinemaSec', N'auto create statistics', N'true'
GO

exec sp_dboption N'CinemaSec', N'auto update statistics', N'true'
GO

use [CinemaSec]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[FK_Rooms_Cinemas]') and OBJECTPROPERTY(id, N'IsForeignKey') = 1)
ALTER TABLE [dbo].[Rooms] DROP CONSTRAINT FK_Rooms_Cinemas
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[FK_Cameras_Rooms]') and OBJECTPROPERTY(id, N'IsForeignKey') = 1)
ALTER TABLE [dbo].[Cameras] DROP CONSTRAINT FK_Cameras_Rooms
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[FK_Statistics_Rooms]') and OBJECTPROPERTY(id, N'IsForeignKey') = 1)
ALTER TABLE [dbo].[Statistics] DROP CONSTRAINT FK_Statistics_Rooms
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[FK_Timetable_Rooms]') and OBJECTPROPERTY(id, N'IsForeignKey') = 1)
ALTER TABLE [dbo].[Timetable] DROP CONSTRAINT FK_Timetable_Rooms
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[FK_StatPhotos_Cameras]') and OBJECTPROPERTY(id, N'IsForeignKey') = 1)
ALTER TABLE [dbo].[StatPhotos] DROP CONSTRAINT FK_StatPhotos_Cameras
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[FK_StatPhotos_Statistics]') and OBJECTPROPERTY(id, N'IsForeignKey') = 1)
ALTER TABLE [dbo].[StatPhotos] DROP CONSTRAINT FK_StatPhotos_Statistics
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[TimePeriods]') and OBJECTPROPERTY(id, N'IsTrigger') = 1)
drop trigger [dbo].[TimePeriods]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[GetNextStatID]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[GetNextStatID]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[TimetableView]') and OBJECTPROPERTY(id, N'IsView') = 1)
drop view [dbo].[TimetableView]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[ExchangeView]') and OBJECTPROPERTY(id, N'IsView') = 1)
drop view [dbo].[ExchangeView]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[StatPhotos]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[StatPhotos]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Cameras]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[Cameras]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Statistics]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[Statistics]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Timetable]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[Timetable]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Rooms]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[Rooms]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Cinemas]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[Cinemas]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Protocol]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[Protocol]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Sequence]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[Sequence]
GO

if not exists (select * from master.dbo.syslogins where loginname = N'CinemaSec')
BEGIN
	declare @logindb nvarchar(132), @loginlang nvarchar(132) select @logindb = N'CinemaSec', @loginlang = N'us_english'
	if @logindb is null or not exists (select * from master.dbo.sysdatabases where name = @logindb)
		select @logindb = N'master'
	if @loginlang is null or (not exists (select * from master.dbo.syslanguages where name = @loginlang) and @loginlang <> N'us_english')
		select @loginlang = @@language
	exec sp_addlogin N'CinemaSec', null, @logindb, @loginlang
END
GO

exec sp_addsrvrolemember N'BUILTIN\Администраторы', sysadmin
GO

exec sp_addsrvrolemember N'CinemaSec', sysadmin
GO

exec sp_addsrvrolemember N'CinemaSec', securityadmin
GO

exec sp_addsrvrolemember N'CinemaSec', serveradmin
GO

exec sp_addsrvrolemember N'CinemaSec', setupadmin
GO

exec sp_addsrvrolemember N'CinemaSec', processadmin
GO

exec sp_addsrvrolemember N'CinemaSec', diskadmin
GO

exec sp_addsrvrolemember N'CinemaSec', dbcreator
GO

exec sp_addsrvrolemember N'CinemaSec', bulkadmin
GO

if not exists (select * from dbo.sysusers where name = N'CinemaSec' and uid < 16382)
	EXEC sp_grantdbaccess N'CinemaSec', N'CinemaSec'
GO

exec sp_addrolemember N'db_accessadmin', N'Cinema'
GO

exec sp_addrolemember N'db_accessadmin', N'CinemaSec'
GO

exec sp_addrolemember N'db_backupoperator', N'Cinema'
GO

exec sp_addrolemember N'db_backupoperator', N'CinemaSec'
GO

exec sp_addrolemember N'db_datareader', N'Cinema'
GO

exec sp_addrolemember N'db_datareader', N'CinemaSec'
GO

exec sp_addrolemember N'db_datawriter', N'Cinema'
GO

exec sp_addrolemember N'db_datawriter', N'CinemaSec'
GO

exec sp_addrolemember N'db_ddladmin', N'Cinema'
GO

exec sp_addrolemember N'db_ddladmin', N'CinemaSec'
GO

exec sp_addrolemember N'db_owner', N'Cinema'
GO

exec sp_addrolemember N'db_owner', N'CinemaSec'
GO

exec sp_addrolemember N'db_securityadmin', N'Cinema'
GO

exec sp_addrolemember N'db_securityadmin', N'CinemaSec'
GO

CREATE TABLE [dbo].[Cinemas] (
	[CinemaID] [int] IDENTITY (1, 1) NOT NULL ,
	[CinemaName] [nvarchar] (50) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL ,
	[CinemaComments] [nvarchar] (50) COLLATE SQL_Latin1_General_CP1_CI_AS NULL ,
	[BackupStart] [float] NOT NULL ,
	[BackupStop] [float] NOT NULL ,
	[BackupDate] [int] NOT NULL ,
	[ExchangeTimeoutNumber] [int] NOT NULL ,
	[ExchangeTimeoutPeriod] [int] NOT NULL ,
	[ExchangeWaitTimeout] [int] NOT NULL ,
	[ExchangeStatTreshold] [int] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[Protocol] (
	[CinemaID] [int] NOT NULL ,
	[ProtoTime] [datetime] NOT NULL ,
	[ProtoType] [smallint] NOT NULL ,
	[ProtoString] [text] COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO

CREATE TABLE [dbo].[Sequence] (
	[StatReport] [int] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[Rooms] (
	[RoomID] [int] IDENTITY (1, 1) NOT NULL ,
	[CinemaID] [int] NOT NULL ,
	[RoomNumber] [int] NOT NULL ,
	[RoomChairs] [image] NULL ,
	[RoomIP] [varchar] (50) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL ,
	[AlgoParams] [image] NULL ,
	[TimeAfterStart] [int] NOT NULL ,
	[TimeBeforeEnd] [int] NOT NULL ,
	[TimePeriod] [int] NOT NULL ,
	[Model] [image] NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO

CREATE TABLE [dbo].[Cameras] (
	[CameraID] [int] IDENTITY (1, 1) NOT NULL ,
	[RoomID] [int] NOT NULL ,
	[CameraType] [smallint] NOT NULL ,
	[CameraCoord] [int] NOT NULL ,
	[CameraUID] [varchar] (80) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL ,
	[CameraIP] [varchar] (50) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL ,
	[CameraBackground] [image] NULL 
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
	[EndTime] [datetime] NOT NULL ,
	[ExchangeID] [int] NOT NULL 
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

ALTER TABLE [dbo].[Cinemas] WITH NOCHECK ADD 
	CONSTRAINT [DF__Cinemas__BackupS__31EC6D26] DEFAULT (50) FOR [BackupStart],
	CONSTRAINT [DF__Cinemas__BackupS__32E0915F] DEFAULT (60) FOR [BackupStop],
	CONSTRAINT [DF__Cinemas__BackupD__33D4B598] DEFAULT (182) FOR [BackupDate],
	CONSTRAINT [DF__Cinemas__Exchang__35BCFE0A] DEFAULT (5) FOR [ExchangeTimeoutNumber],
	CONSTRAINT [DF__Cinemas__Exchang__36B12243] DEFAULT (60) FOR [ExchangeTimeoutPeriod],
	CONSTRAINT [DF_Cinemas_ExchangeWaitTimeout] DEFAULT (300) FOR [ExchangeWaitTimeout],
	CONSTRAINT [DF_Cinemas_ExchangeStatTreshold] DEFAULT (300) FOR [ExchangeStatTreshold],
	CONSTRAINT [CK_Cinemas] CHECK ([BackupStart] >= 0 and [BackupStart] < 100 and [BackupStop] >= 0 and [BackupStop] < 100)
GO

ALTER TABLE [dbo].[Sequence] WITH NOCHECK ADD 
	CONSTRAINT [DF_Sequence_StatReport] DEFAULT (1) FOR [StatReport]
GO

ALTER TABLE [dbo].[Rooms] WITH NOCHECK ADD 
	CONSTRAINT [DF_Rooms_RoomChairs] DEFAULT (null) FOR [RoomChairs],
	CONSTRAINT [DF_Rooms_AlgoParams] DEFAULT (null) FOR [AlgoParams],
	CONSTRAINT [DF_Rooms_TimeAfterStart] DEFAULT (600) FOR [TimeAfterStart],
	CONSTRAINT [DF_Rooms_TimeBeforeEnd] DEFAULT (600) FOR [TimeBeforeEnd],
	CONSTRAINT [DF_Rooms_TimePeriod] DEFAULT (600) FOR [TimePeriod]
GO

ALTER TABLE [dbo].[Cameras] WITH NOCHECK ADD 
	CONSTRAINT [DF_Cameras_CameraType] DEFAULT (0) FOR [CameraType],
	CONSTRAINT [DF_Cameras_CameraCoord] DEFAULT (0) FOR [CameraCoord]
GO

ALTER TABLE [dbo].[Timetable] WITH NOCHECK ADD 
	CONSTRAINT [DF__Timetable__Excha__38996AB5] DEFAULT ((-1)) FOR [ExchangeID]
GO

GRANT  SELECT ,  UPDATE ,  INSERT ,  DELETE  ON [dbo].[Cinemas]  TO [CinemaSec]
GO

GRANT  SELECT ,  UPDATE ,  INSERT ,  DELETE  ON [dbo].[Protocol]  TO [CinemaSec]
GO

GRANT  SELECT ,  UPDATE ,  INSERT ,  DELETE  ON [dbo].[Rooms]  TO [CinemaSec]
GO

GRANT  SELECT ,  UPDATE ,  INSERT ,  DELETE  ON [dbo].[Cameras]  TO [CinemaSec]
GO

GRANT  SELECT ,  UPDATE ,  INSERT ,  DELETE  ON [dbo].[Statistics]  TO [CinemaSec]
GO

GRANT  SELECT ,  UPDATE ,  INSERT ,  DELETE  ON [dbo].[Timetable]  TO [CinemaSec]
GO

GRANT  SELECT ,  UPDATE ,  INSERT ,  DELETE  ON [dbo].[StatPhotos]  TO [CinemaSec]
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

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO


-- trigger

CREATE VIEW dbo.ExchangeView
AS
SELECT     dbo.Rooms.RoomID, dbo.Cinemas.ExchangeTimeoutNumber, dbo.Cinemas.ExchangeTimeoutPeriod,
				 dbo.Cinemas.ExchangeWaitTimeout, dbo.Cinemas.ExchangeStatTreshold
FROM         dbo.Rooms INNER JOIN
                      dbo.Cinemas ON dbo.Rooms.CinemaID = dbo.Cinemas.CinemaID



GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

GRANT  SELECT ,  UPDATE ,  INSERT ,  DELETE  ON [dbo].[ExchangeView]  TO [CinemaSec]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO


/****** Object:  View dbo.TimetableView    Script Date: 17.06.2005 12:20:05 ******/
CREATE VIEW dbo.TimetableView
AS
SELECT     dbo.Cinemas.CinemaName, dbo.Rooms.RoomNumber, dbo.Timetable.FilmID, dbo.Timetable.BeginTime, dbo.Timetable.EndTime, dbo.Timetable.Name, 
                      dbo.Timetable.ExchangeID
FROM         dbo.Timetable INNER JOIN
                      dbo.Rooms ON dbo.Timetable.RoomID = dbo.Rooms.RoomID INNER JOIN
                      dbo.Cinemas ON dbo.Rooms.CinemaID = dbo.Cinemas.CinemaID


GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

GRANT  SELECT ,  UPDATE ,  INSERT ,  DELETE  ON [dbo].[TimetableView]  TO [CinemaSec]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS OFF 
GO

CREATE    PROCEDURE dbo.GetNextStatID
-- @NextStatID int output
as
begin
	declare @NextStatID  int
	DECLARE stat_cursor CURSOR
	FOR select StatReport from [Sequence]

	open stat_cursor
	fetch from stat_cursor into @NextStatID;
	if @NextStatID >= 2147483647
	begin
		set  @NextStatID = 1
	end
	UPDATE [Sequence]
	set StatReport= @NextStatID + 1
	where current of stat_cursor
	CLOSE stat_cursor
	DEALLOCATE stat_cursor
return  @NextStatID

end
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO


CREATE   TRIGGER TimePeriods
ON dbo.Timetable
FOR INSERT, UPDATE 
AS
DECLARE @BeginTime datetime, @EndTime datetime
SELECT @BeginTime = BeginTime, 
   @EndTime = EndTime 
FROM timetable
IF @BeginTime >= @EndTime
BEGIN
   RAISERROR (50100, 16, 1)
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
	RAISERROR (50101, 16, 1)
	ROLLBACK TRANSACTION
	BREAK
	END
SET @BeginTimePrev = @BeginTimeNext
SET @EndTimePrev = @EndTimeNext
END

END

CLOSE TimeCursor
DEALLOCATE TimeCursor

-- end trigger


GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

