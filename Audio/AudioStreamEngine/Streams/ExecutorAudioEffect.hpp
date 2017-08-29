#pragma once
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <TAudioStream.h>
#include <iscore/tools/Todo.hpp>


class TExecutorAudioStream final : public TDecoratedAudioStream
{
private:
  ossia::time_constraint& m_root;
public:
  TExecutorAudioStream(TAudioStreamPtr stream, ossia::time_constraint& cst):
    m_root{cst}
  {
    fStream = stream;
  }

  long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override
  {
    try {
      m_root.tick(ossia::time_value(framesNum));
    }
    catch(...)
    {
      ISCORE_TODO;
    }
    return fStream->Read(buffer, framesNum, framePos);
  }

  long Write(FLOAT_BUFFER buffer, long framesNum, long framePos) override
  {
    return 0;
  }

  long SetPos(long frames) override
  {
      m_root.offset(ossia::time_value(frames));
      return fStream->SetPos(frames);
  }

  TAudioStreamPtr CutBegin(long frames) override
  {
    return {};
  }

  void Reset() override
  {
    fStream->Reset();
  }

  TAudioStreamPtr Copy() override
  {
    return {};
  }
};
