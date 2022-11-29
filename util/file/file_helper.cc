// Copyright 2020 The Crashpad Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "third_party/zlib/zlib/zlib.h"
#include "util/file/file_helper.h"

namespace crashpad {

void CopyFileContent(FileReaderInterface* file_reader,
                     FileWriterInterface* file_writer) {
  char buf[4096];
  FileOperationResult read_result;
  do {
    read_result = file_reader->Read(buf, sizeof(buf));
    if (read_result < 0) {
      break;
    }
    if (read_result > 0 && !file_writer->Write(buf, read_result)) {
      break;
    }
  } while (read_result > 0);
}

void CopyCompressFileContent(FileReaderInterface* file_reader,
                             FileWriterInterface* file_writer) {
  char buf[4096];
  char output[4096];
  FileOperationResult read_result;
  z_stream zs;
  zs.zalloc = Z_NULL;
  zs.zfree = Z_NULL;
  zs.opaque = Z_NULL;
  do {
    read_result = file_reader->Read(buf, sizeof(buf));
    if (read_result < 0) {
      break;
    }
    zs.avail_in = (uInt)read_result;
    zs.next_in = (Bytef*)buf;
    zs.avail_out = (uInt)sizeof(output);
    zs.next_out = (Bytef*)output;
    deflateInit2(
        &zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 | 16, 8, Z_DEFAULT_STRATEGY);
    deflate(&zs, Z_FINISH);
    deflateEnd(&zs);
    if (read_result > 0 && !file_writer->Write(output, zs.total_out)) {
      break;
    }
  } while (read_result > 0);
}

}  // namespace crashpad
