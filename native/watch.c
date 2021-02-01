#include <CoreServices/CoreServices.h>
#include <stdio.h>

void myCallbackFunction(ConstFSEventStreamRef streamRef,
                        void *clientCallBackInfo, size_t numEvents,
                        void *eventPaths,
                        const FSEventStreamEventFlags eventFlags[],
                        const FSEventStreamEventId eventIds[]) {

  char **paths = eventPaths;
  for (int i = 0; i < numEvents; i++) {
    const char *event = "?";
    if (eventFlags[i] & kFSEventStreamEventFlagItemRemoved) {
      event = "removed";
    } else if (eventFlags[i] & kFSEventStreamEventFlagItemRenamed) {
      event = "moved";
    } else if (eventFlags[i] & kFSEventStreamEventFlagItemModified) {
      event = "modified";
    } else if (eventFlags[i] & kFSEventStreamEventFlagItemCreated) {
      event = "created";
    } else if (eventFlags[i] & kFSEventStreamEventFlagItemRenamed) {
      event = "renamed";
    } else if (eventFlags[i] & kFSEventStreamEventFlagItemInodeMetaMod) {
      event = "meta";
    }

    const char *type = "?";
    if ((eventFlags[i] & kFSEventStreamEventFlagItemIsFile) == kFSEventStreamEventFlagItemIsFile) {
      type = "file";
    } else if ((eventFlags[i] & kFSEventStreamEventFlagItemIsDir) == kFSEventStreamEventFlagItemIsDir) {
      type = "dir";
    } else if ((eventFlags[i] & kFSEventStreamEventFlagItemIsSymlink) == kFSEventStreamEventFlagItemIsSymlink) {
      type = "symlink";
    }

    printf("[%s %s]: %s\n", event, type, paths[i]);
  }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: watch <path>\n");
        return 1;
    }

    printf("Monitoring %s\n", argv[1]);
    CFStringRef path =
        CFStringCreateWithCString(NULL, argv[1], kCFStringEncodingMacRoman);
    CFArrayRef pathsToWatch =
        CFArrayCreate(NULL, (const void **)&path, 1, NULL);
    void *callbackInfo = NULL;
    CFAbsoluteTime latency = 0.1;  // TODO: this matches the js one.

    FSEventStreamRef stream = FSEventStreamCreate(
        NULL, &myCallbackFunction, callbackInfo, pathsToWatch,
        kFSEventStreamEventIdSinceNow, latency,
        kFSEventStreamCreateFlagNone | kFSEventStreamCreateFlagWatchRoot |
            kFSEventStreamCreateFlagFileEvents);

    CFRunLoopRef loop = CFRunLoopGetCurrent();
    CFRunLoopPerformBlock(loop, kCFRunLoopDefaultMode, ^(void) {
      FSEventStreamScheduleWithRunLoop(stream, loop, kCFRunLoopDefaultMode);
      FSEventStreamStart(stream);
    });
    CFRunLoopRun();

    CFRelease(path);
    return 0;
}
