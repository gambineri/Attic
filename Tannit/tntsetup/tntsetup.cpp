/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#include <windows.h>
#include <stdio.h>

#define OPT_REBOOT    "/REBOOT"


void main(int argc, char** argv)
{
  if (argc <= 2)
  {
    printf("USAGE: \n"
           "tntsetup file_to_move [dest_file_path] [%s]\n\n"
           "Notes: passing only the first parameter forces its DELETION;\n"
           "       if 'dest_file_path' represents a really existing file, it will'be scratched.\n"
           "       in the %s case, the operation is written to the registry in:\n"
           "       HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\PendingFileRenameOperations\n\n"
           "%s: schedules the operation at the next reboot.\n", OPT_REBOOT, OPT_REBOOT, OPT_REBOOT);
    return;
  }

  try
  {
    SetLastError(0);

    DWORD flags = MOVEFILE_REPLACE_EXISTING;
    if (argc >= 4 && strcmp(argv[3], OPT_REBOOT) == 0)
      flags |= MOVEFILE_DELAY_UNTIL_REBOOT;
    else
      flags |= MOVEFILE_COPY_ALLOWED;

    char* existing = argv[1];
    char* dest     = (argc >= 3 ? argv[2] : NULL);

    if (MoveFileEx(existing, dest, flags) == FALSE)
      printf("MoveFileEx failed: GetLastError returns %d.\n", GetLastError());
  }
  catch(...)
  {
    printf("Generic unhandled exception during execution of core %s\n"
           "GetLastError returns %d.\n", argv[0], GetLastError());
  }
}

