import datetime
import shutil
import os

start_cam_id = 19
end_cam_id = 21

start_date = datetime.date( 2005,12,15 )
end_date = datetime.date( 2006,01,05 )
path = "./"
template_src = path + "template.avi"

if( not os.path.exists( "./CinemaSec" ) ):
    os.mkdir( "./CinemaSec" )

current_cam_id = start_cam_id
while ( current_cam_id <= end_cam_id ):
    current_date = start_date
    
    #Make dir
    current_cam_path = path + "CinemaSec/" + str( current_cam_id )
    if ( not os.path.exists( current_cam_path) ):
        os.mkdir( current_cam_path )
    
    while current_date <= end_date:
        
        current_cam_date_path = current_cam_path + "/" + current_date.strftime( "%y-%m-%d" )
        if ( not os.path.exists(current_cam_date_path) ):
            os.mkdir( current_cam_date_path )
        
        current_time = datetime.datetime( current_date.year, current_date.month, current_date.day, 00,00,00 )
        end_time = datetime.datetime ( current_date.year, current_date.month, current_date.day, 23,59,59 )
    
        while( current_time <= end_time ):
            current_cam_date_time_path = current_cam_date_path + "/" + current_time.time().strftime("%H-%M-%S")  + ".avi"
            
            current_time += datetime.timedelta( minutes = 10, seconds = 30 )
            shutil.copy( template_src, current_cam_date_time_path )
        
        current_date += datetime.timedelta( days=1 )
    
    current_cam_id += 1


    
    
