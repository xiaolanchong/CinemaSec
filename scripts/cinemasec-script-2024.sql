USE [master]
GO
/****** Object:  Database [CinemaSec2016]    Script Date: 4/7/2024 9:22:56 PM ******/
CREATE DATABASE [CinemaSec2016]
 CONTAINMENT = NONE
 ON  PRIMARY 
( NAME = N'CinemaSec', FILENAME = N'E:\CinemaSec2016.mdf' , SIZE = 1772544KB , MAXSIZE = UNLIMITED, FILEGROWTH = 10%)
 LOG ON 
( NAME = N'SKZ_log', FILENAME = N'E:\CinemaSec2016_0.LDF' , SIZE = 178112KB , MAXSIZE = UNLIMITED, FILEGROWTH = 10%)
 WITH CATALOG_COLLATION = DATABASE_DEFAULT, LEDGER = OFF
GO
ALTER DATABASE [CinemaSec2016] SET COMPATIBILITY_LEVEL = 100
GO
IF (1 = FULLTEXTSERVICEPROPERTY('IsFullTextInstalled'))
begin
EXEC [CinemaSec2016].[dbo].[sp_fulltext_database] @action = 'disable'
end
GO
ALTER DATABASE [CinemaSec2016] SET ANSI_NULL_DEFAULT OFF 
GO
ALTER DATABASE [CinemaSec2016] SET ANSI_NULLS OFF 
GO
ALTER DATABASE [CinemaSec2016] SET ANSI_PADDING OFF 
GO
ALTER DATABASE [CinemaSec2016] SET ANSI_WARNINGS OFF 
GO
ALTER DATABASE [CinemaSec2016] SET ARITHABORT OFF 
GO
ALTER DATABASE [CinemaSec2016] SET AUTO_CLOSE OFF 
GO
ALTER DATABASE [CinemaSec2016] SET AUTO_SHRINK OFF 
GO
ALTER DATABASE [CinemaSec2016] SET AUTO_UPDATE_STATISTICS ON 
GO
ALTER DATABASE [CinemaSec2016] SET CURSOR_CLOSE_ON_COMMIT OFF 
GO
ALTER DATABASE [CinemaSec2016] SET CURSOR_DEFAULT  GLOBAL 
GO
ALTER DATABASE [CinemaSec2016] SET CONCAT_NULL_YIELDS_NULL OFF 
GO
ALTER DATABASE [CinemaSec2016] SET NUMERIC_ROUNDABORT OFF 
GO
ALTER DATABASE [CinemaSec2016] SET QUOTED_IDENTIFIER OFF 
GO
ALTER DATABASE [CinemaSec2016] SET RECURSIVE_TRIGGERS OFF 
GO
ALTER DATABASE [CinemaSec2016] SET  DISABLE_BROKER 
GO
ALTER DATABASE [CinemaSec2016] SET AUTO_UPDATE_STATISTICS_ASYNC OFF 
GO
ALTER DATABASE [CinemaSec2016] SET DATE_CORRELATION_OPTIMIZATION OFF 
GO
ALTER DATABASE [CinemaSec2016] SET TRUSTWORTHY OFF 
GO
ALTER DATABASE [CinemaSec2016] SET ALLOW_SNAPSHOT_ISOLATION OFF 
GO
ALTER DATABASE [CinemaSec2016] SET PARAMETERIZATION SIMPLE 
GO
ALTER DATABASE [CinemaSec2016] SET READ_COMMITTED_SNAPSHOT OFF 
GO
ALTER DATABASE [CinemaSec2016] SET HONOR_BROKER_PRIORITY OFF 
GO
ALTER DATABASE [CinemaSec2016] SET RECOVERY FULL 
GO
ALTER DATABASE [CinemaSec2016] SET  MULTI_USER 
GO
ALTER DATABASE [CinemaSec2016] SET PAGE_VERIFY TORN_PAGE_DETECTION  
GO
ALTER DATABASE [CinemaSec2016] SET DB_CHAINING OFF 
GO
ALTER DATABASE [CinemaSec2016] SET FILESTREAM( NON_TRANSACTED_ACCESS = OFF ) 
GO
ALTER DATABASE [CinemaSec2016] SET TARGET_RECOVERY_TIME = 0 SECONDS 
GO
ALTER DATABASE [CinemaSec2016] SET DELAYED_DURABILITY = DISABLED 
GO
ALTER DATABASE [CinemaSec2016] SET ACCELERATED_DATABASE_RECOVERY = OFF  
GO
ALTER DATABASE [CinemaSec2016] SET QUERY_STORE = OFF
GO
USE [CinemaSec2016]
GO
/****** Object:  User [CinemaSec]    Script Date: 4/7/2024 9:22:56 PM ******/
CREATE USER [CinemaSec] WITHOUT LOGIN WITH DEFAULT_SCHEMA=[CinemaSec]
GO
/****** Object:  User [cinema]    Script Date: 4/7/2024 9:22:56 PM ******/
CREATE USER [cinema] WITHOUT LOGIN WITH DEFAULT_SCHEMA=[cinema]
GO
ALTER ROLE [db_owner] ADD MEMBER [CinemaSec]
GO
ALTER ROLE [db_accessadmin] ADD MEMBER [CinemaSec]
GO
ALTER ROLE [db_securityadmin] ADD MEMBER [CinemaSec]
GO
ALTER ROLE [db_ddladmin] ADD MEMBER [CinemaSec]
GO
ALTER ROLE [db_backupoperator] ADD MEMBER [CinemaSec]
GO
ALTER ROLE [db_datareader] ADD MEMBER [CinemaSec]
GO
ALTER ROLE [db_datawriter] ADD MEMBER [CinemaSec]
GO
ALTER ROLE [db_owner] ADD MEMBER [cinema]
GO
ALTER ROLE [db_datareader] ADD MEMBER [cinema]
GO
ALTER ROLE [db_datawriter] ADD MEMBER [cinema]
GO
/****** Object:  Schema [cinema]    Script Date: 4/7/2024 9:22:56 PM ******/
CREATE SCHEMA [cinema]
GO
/****** Object:  Schema [CinemaSec]    Script Date: 4/7/2024 9:22:56 PM ******/
CREATE SCHEMA [CinemaSec]
GO
/****** Object:  Table [dbo].[Rooms]    Script Date: 4/7/2024 9:22:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Rooms](
	[RoomID] [int] IDENTITY(1,1) NOT NULL,
	[CinemaID] [int] NOT NULL,
	[RoomNumber] [int] NOT NULL,
	[RoomChairs] [varbinary](max) NULL,
	[RoomIP] [varchar](50) NOT NULL,
	[AlgoParams] [varbinary](max) NULL,
	[TimeAfterStart] [int] NOT NULL,
	[TimeBeforeEnd] [int] NOT NULL,
	[TimePeriod] [int] NOT NULL,
	[Model] [varbinary](max) NULL,
 CONSTRAINT [PK_Rooms] PRIMARY KEY CLUSTERED 
(
	[RoomID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Cinemas]    Script Date: 4/7/2024 9:22:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Cinemas](
	[CinemaID] [int] IDENTITY(1,1) NOT NULL,
	[CinemaName] [nvarchar](50) NOT NULL,
	[CinemaComments] [nvarchar](50) NULL,
	[BackupStart] [float] NOT NULL,
	[BackupStop] [float] NOT NULL,
	[BackupDate] [int] NOT NULL,
	[ExchangeTimeoutNumber] [int] NOT NULL,
	[ExchangeTimeoutPeriod] [int] NOT NULL,
	[ExchangeWaitTimeout] [int] NOT NULL,
	[ExchangeStatTreshold] [int] NOT NULL,
 CONSTRAINT [PK_Cinemas] PRIMARY KEY CLUSTERED 
(
	[CinemaID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  View [dbo].[ExchangeView]    Script Date: 4/7/2024 9:22:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO


-- trigger

CREATE VIEW [dbo].[ExchangeView]
AS
SELECT     dbo.Rooms.RoomID, dbo.Cinemas.ExchangeTimeoutNumber, dbo.Cinemas.ExchangeTimeoutPeriod,
				 dbo.Cinemas.ExchangeWaitTimeout, dbo.Cinemas.ExchangeStatTreshold
FROM         dbo.Rooms INNER JOIN
                      dbo.Cinemas ON dbo.Rooms.CinemaID = dbo.Cinemas.CinemaID



GO
/****** Object:  Table [dbo].[Timetable]    Script Date: 4/7/2024 9:22:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Timetable](
	[FilmID] [int] IDENTITY(1,1) NOT NULL,
	[RoomID] [int] NOT NULL,
	[Name] [nvarchar](500) NOT NULL,
	[BeginTime] [datetime] NOT NULL,
	[EndTime] [datetime] NOT NULL,
	[ExchangeID] [int] NOT NULL,
 CONSTRAINT [PK_Timetable] PRIMARY KEY CLUSTERED 
(
	[FilmID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  View [dbo].[TimetableView]    Script Date: 4/7/2024 9:22:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/****** Object:  View dbo.TimetableView    Script Date: 17.06.2005 12:20:05 ******/
CREATE VIEW [dbo].[TimetableView]
AS
SELECT     dbo.Cinemas.CinemaName, dbo.Rooms.RoomNumber, dbo.Timetable.FilmID, dbo.Timetable.BeginTime, dbo.Timetable.EndTime, dbo.Timetable.Name, 
                      dbo.Timetable.ExchangeID
FROM         dbo.Timetable INNER JOIN
                      dbo.Rooms ON dbo.Timetable.RoomID = dbo.Rooms.RoomID INNER JOIN
                      dbo.Cinemas ON dbo.Rooms.CinemaID = dbo.Cinemas.CinemaID


GO
/****** Object:  Table [dbo].[Cameras]    Script Date: 4/7/2024 9:22:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Cameras](
	[CameraID] [int] IDENTITY(1,1) NOT NULL,
	[RoomID] [int] NOT NULL,
	[CameraType] [smallint] NOT NULL,
	[CameraCoord] [int] NOT NULL,
	[CameraUID] [varchar](80) NOT NULL,
	[CameraIP] [varchar](50) NOT NULL,
	[CameraBackground] [varbinary](max) NULL,
 CONSTRAINT [PK_Cameras] PRIMARY KEY CLUSTERED 
(
	[CameraID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Protocol]    Script Date: 4/7/2024 9:22:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Protocol](
	[CinemaID] [int] NOT NULL,
	[ProtoTime] [datetime] NOT NULL,
	[ProtoType] [smallint] NOT NULL,
	[ProtoString] [text] NOT NULL
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Results]    Script Date: 4/7/2024 9:22:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Results](
	[RoomID] [int] NOT NULL,
	[CameraType] [smallint] NOT NULL,
	[CameraCoord] [int] NOT NULL,
	[CameraUID] [varchar](80) NOT NULL,
	[CameraIP] [varchar](50) NOT NULL,
	[CameraBackground] [varbinary](max) NULL
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Sequence]    Script Date: 4/7/2024 9:22:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Sequence](
	[StatReport] [int] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Statistics]    Script Date: 4/7/2024 9:22:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Statistics](
	[StatID] [int] IDENTITY(1,1) NOT NULL,
	[RoomID] [int] NOT NULL,
	[StatTime] [datetime] NOT NULL,
	[StatViewerNumber] [smallint] NOT NULL,
 CONSTRAINT [PK_Statistics] PRIMARY KEY CLUSTERED 
(
	[StatID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[StatPhotos]    Script Date: 4/7/2024 9:22:56 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[StatPhotos](
	[StatID] [int] NOT NULL,
	[CameraID] [int] NOT NULL,
	[StatPhoto] [varbinary](max) NULL,
 CONSTRAINT [PK_StatPhotos] PRIMARY KEY CLUSTERED 
(
	[StatID] ASC,
	[CameraID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO
ALTER TABLE [dbo].[Cameras] ADD  CONSTRAINT [DF_Cameras_CameraType]  DEFAULT ((0)) FOR [CameraType]
GO
ALTER TABLE [dbo].[Cameras] ADD  CONSTRAINT [DF_Cameras_CameraCoord]  DEFAULT ((0)) FOR [CameraCoord]
GO
ALTER TABLE [dbo].[Cinemas] ADD  CONSTRAINT [DF__Cinemas__BackupS__31EC6D26]  DEFAULT (50) FOR [BackupStart]
GO
ALTER TABLE [dbo].[Cinemas] ADD  CONSTRAINT [DF__Cinemas__BackupS__32E0915F]  DEFAULT (60) FOR [BackupStop]
GO
ALTER TABLE [dbo].[Cinemas] ADD  CONSTRAINT [DF__Cinemas__BackupD__33D4B598]  DEFAULT (182) FOR [BackupDate]
GO
ALTER TABLE [dbo].[Cinemas] ADD  CONSTRAINT [DF__Cinemas__Exchang__35BCFE0A]  DEFAULT (5) FOR [ExchangeTimeoutNumber]
GO
ALTER TABLE [dbo].[Cinemas] ADD  CONSTRAINT [DF__Cinemas__Exchang__36B12243]  DEFAULT (60) FOR [ExchangeTimeoutPeriod]
GO
ALTER TABLE [dbo].[Cinemas] ADD  CONSTRAINT [DF_Cinemas_ExchangeWaitTimeout]  DEFAULT (300) FOR [ExchangeWaitTimeout]
GO
ALTER TABLE [dbo].[Cinemas] ADD  CONSTRAINT [DF_Cinemas_ExchangeStatTreshold]  DEFAULT (300) FOR [ExchangeStatTreshold]
GO
ALTER TABLE [dbo].[Rooms] ADD  CONSTRAINT [DF_Rooms_RoomChairs]  DEFAULT (NULL) FOR [RoomChairs]
GO
ALTER TABLE [dbo].[Rooms] ADD  CONSTRAINT [DF_Rooms_AlgoParams]  DEFAULT (NULL) FOR [AlgoParams]
GO
ALTER TABLE [dbo].[Rooms] ADD  CONSTRAINT [DF_Rooms_TimeAfterStart]  DEFAULT ((600)) FOR [TimeAfterStart]
GO
ALTER TABLE [dbo].[Rooms] ADD  CONSTRAINT [DF_Rooms_TimeBeforeEnd]  DEFAULT ((600)) FOR [TimeBeforeEnd]
GO
ALTER TABLE [dbo].[Rooms] ADD  CONSTRAINT [DF_Rooms_TimePeriod]  DEFAULT ((600)) FOR [TimePeriod]
GO
ALTER TABLE [dbo].[Sequence] ADD  CONSTRAINT [DF_Sequence_StatReport]  DEFAULT (1) FOR [StatReport]
GO
ALTER TABLE [dbo].[Timetable] ADD  CONSTRAINT [DF__Timetable__Excha__38996AB5]  DEFAULT ((-1)) FOR [ExchangeID]
GO
ALTER TABLE [dbo].[Cameras]  WITH NOCHECK ADD  CONSTRAINT [FK_Cameras_Rooms] FOREIGN KEY([RoomID])
REFERENCES [dbo].[Rooms] ([RoomID])
ON UPDATE CASCADE
ON DELETE CASCADE
GO
ALTER TABLE [dbo].[Cameras] CHECK CONSTRAINT [FK_Cameras_Rooms]
GO
ALTER TABLE [dbo].[Rooms]  WITH NOCHECK ADD  CONSTRAINT [FK_Rooms_Cinemas] FOREIGN KEY([CinemaID])
REFERENCES [dbo].[Cinemas] ([CinemaID])
ON UPDATE CASCADE
ON DELETE CASCADE
GO
ALTER TABLE [dbo].[Rooms] CHECK CONSTRAINT [FK_Rooms_Cinemas]
GO
ALTER TABLE [dbo].[Statistics]  WITH NOCHECK ADD  CONSTRAINT [FK_Statistics_Rooms] FOREIGN KEY([RoomID])
REFERENCES [dbo].[Rooms] ([RoomID])
ON UPDATE CASCADE
ON DELETE CASCADE
GO
ALTER TABLE [dbo].[Statistics] CHECK CONSTRAINT [FK_Statistics_Rooms]
GO
ALTER TABLE [dbo].[StatPhotos]  WITH CHECK ADD  CONSTRAINT [FK_StatPhotos_Cameras] FOREIGN KEY([CameraID])
REFERENCES [dbo].[Cameras] ([CameraID])
GO
ALTER TABLE [dbo].[StatPhotos] CHECK CONSTRAINT [FK_StatPhotos_Cameras]
GO
ALTER TABLE [dbo].[StatPhotos]  WITH CHECK ADD  CONSTRAINT [FK_StatPhotos_Statistics] FOREIGN KEY([StatID])
REFERENCES [dbo].[Statistics] ([StatID])
ON UPDATE CASCADE
ON DELETE CASCADE
GO
ALTER TABLE [dbo].[StatPhotos] CHECK CONSTRAINT [FK_StatPhotos_Statistics]
GO
ALTER TABLE [dbo].[Timetable]  WITH NOCHECK ADD  CONSTRAINT [FK_Timetable_Rooms] FOREIGN KEY([RoomID])
REFERENCES [dbo].[Rooms] ([RoomID])
ON UPDATE CASCADE
ON DELETE CASCADE
GO
ALTER TABLE [dbo].[Timetable] CHECK CONSTRAINT [FK_Timetable_Rooms]
GO
ALTER TABLE [dbo].[Cinemas]  WITH NOCHECK ADD  CONSTRAINT [CK_Cinemas] CHECK  (([BackupStart] >= 0 and [BackupStart] < 100 and [BackupStop] >= 0 and [BackupStop] < 100))
GO
ALTER TABLE [dbo].[Cinemas] CHECK CONSTRAINT [CK_Cinemas]
GO
/****** Object:  StoredProcedure [dbo].[GetNextStatID]    Script Date: 4/7/2024 9:22:56 PM ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE    PROCEDURE [dbo].[GetNextStatID]
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
USE [master]
GO
ALTER DATABASE [CinemaSec2016] SET  READ_WRITE 
GO
