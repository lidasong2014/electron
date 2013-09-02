// Copyright (c) 2013 GitHub, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string.h>

#include "content/public/app/content_main.h"

namespace node {
int Start(int argc, char *argv[]);
}

#if defined(OS_WIN)

#include <windows.h>  // NOLINT
#include <shellapi.h>  // NOLINT

#include "app/atom_main_delegate.h"
#include "content/public/app/startup_helper_win.h"
#include "sandbox/win/src/sandbox_types.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, wchar_t* cmd, int) {
  int argc = 0;
  wchar_t** wargv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
  if (argc > 1 && wcscmp(wargv[1], L"--atom-child_process-fork") == 0) {
    // Convert argv to to UTF8
    char** argv = new char*[argc];
    for (int i = 0; i < argc; i++) {
      // Compute the size of the required buffer
      DWORD size = WideCharToMultiByte(CP_UTF8,
                                       0,
                                       wargv[i],
                                       -1,
                                       NULL,
                                       0,
                                       NULL,
                                       NULL);
      if (size == 0) {
        // This should never happen.
        fprintf(stderr, "Could not convert arguments to utf8.");
        exit(1);
      }
      // Do the actual conversion
      argv[i] = new char[size];
      DWORD result = WideCharToMultiByte(CP_UTF8,
                                         0,
                                         wargv[i],
                                         -1,
                                         argv[i],
                                         size,
                                         NULL,
                                         NULL);
      if (result == 0) {
        // This should never happen.
        fprintf(stderr, "Could not convert arguments to utf8.");
        exit(1);
      }
    }
    // Now that conversion is done, we can finally start.
    argv[1] = argv[0];
    return node::Start(argc - 1, argv + 1);
  }

  sandbox::SandboxInterfaceInfo sandbox_info = {0};
  content::InitializeSandboxInfo(&sandbox_info);
  atom::AtomMainDelegate delegate;
  return content::ContentMain(instance, &sandbox_info, &delegate);
}

#else  // defined(OS_WIN)

#include "app/atom_library_main.h"

int main(int argc, const char* argv[]) {
  if (argc > 1 && strcmp(argv[1], "--atom-child_process-fork") == 0) {
    argv[1] = argv[0];
    return node::Start(argc - 1, const_cast<char**>(argv + 1));
  }

  return AtomMain(argc, argv);
}

#endif
