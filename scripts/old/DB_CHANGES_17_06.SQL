ALTER TABLE [dbo].[Cinemas]
	ADD [BackupStart] [float] NOT NULL  DEFAULT (50),	
	 [BackupStop] [float] NOT NULL DEFAULT (60),
	 [BackupDate] [int] NOT NULL DEFAULT (182),
	 [ExchangeStartOfDay] [datetime] NOT NULL DEFAULT ('10:00'),
	 [ExchangeTimeoutNumber] [int] NOT NULL DEFAULT (5),
	 [ExchangeTimeoutPeriod] [int] NOT NULL DEFAULT (60) 

GO

ALTER TABLE [dbo].[Cinemas] WITH NOCHECK ADD 
	CONSTRAINT [CK_Cinemas] CHECK ([BackupStart] >= 0 and [BackupStart] < 100 and [BackupStop] >= 0 and [BackupStop] < 100)


ALTER Table [dbo].[Rooms] WITH NOCHECK
	ADD	[Model] [image] NULL

ALTER Table [dbo].[Rooms] 
	ALTER COLUMN [RoomNumber] [int] NOT NULL

ALTER Table [dbo].[Rooms]
	DROP 	CONSTRAINT DF_Rooms_BackupStart,
		CONSTRAINT DF_Rooms_BackupEnd,
		CONSTRAINT DF_Rooms_BackupDate,
	 	CONSTRAINT CK_Rooms

GO

ALTER Table [dbo].[Rooms]
	DROP COLUMN 	[BackupStart] ,
			[BackupEnd] ,
			[BackupDate] 

ALTER Table [dbo].[Cameras] 
	ALTER COLUMN [CameraIP] [varchar] (50) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL

GO

ALTER Table [dbo].[Timetable] WITH NOCHECK
	ADD	[ExchangeID] [int] NOT NULL  DEFAULT ((-1))

GO

ALTER TABLE [dbo].[Rooms] 
	DROP CONSTRAINT [DF_Rooms_TimeAfterStart],
	  CONSTRAINT [DF_Rooms_TimeBeforeEnd],
	  CONSTRAINT [DF_Rooms_TimePeriod]
GO

ALTER TABLE [dbo].[Rooms] 
	ADD CONSTRAINT [DF_Rooms_TimeAfterStart] DEFAULT (600) FOR [TimeAfterStart],
	 CONSTRAINT [DF_Rooms_TimeBeforeEnd] DEFAULT (600) FOR [TimeBeforeEnd],
	 CONSTRAINT [DF_Rooms_TimePeriod] DEFAULT (600) FOR [TimePeriod]
GO

ALTER TABLE [dbo].[Cameras] WITH NOCHECK ADD 
	CONSTRAINT [DF_Cameras_CameraType] DEFAULT (0) FOR [CameraType],
	CONSTRAINT [DF_Cameras_CameraCoord] DEFAULT (0) FOR [CameraCoord]
GO

-- trigger

CREATE VIEW dbo.ExchangeView
AS
SELECT     dbo.Rooms.RoomID, dbo.Cinemas.ExchangeStartOfDay, dbo.Cinemas.ExchangeTimeoutNumber, dbo.Cinemas.ExchangeTimeoutPeriod
FROM         dbo.Rooms INNER JOIN
                      dbo.Cinemas ON dbo.Rooms.CinemaID = dbo.Cinemas.CinemaID

GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
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

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

exec sp_addmessage 50100 , 16, N'Invalid time interval - begin time larger or equal to end one' 
exec sp_addmessage 50101 , 16, N'Intersection beetwen different time intervals'

-- start trigger 

DROP TRIGGER TimePeriods

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
