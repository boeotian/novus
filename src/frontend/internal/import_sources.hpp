#pragma once
#include "filesystem.hpp"
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "parse/node_visitor_optional.hpp"
#include <forward_list>
#include <optional>
#include <string>
#include <vector>

namespace frontend::internal {

class ImportSources final : public parse::OptionalNodeVisitor {
  using path = typename filesystem::path;

public:
  ImportSources() = delete;
  ImportSources(
      const Source& mainSource,
      const std::vector<path>& searchPaths,
      std::forward_list<Source>* importedSources,
      std::vector<Diag>* diags);
  ImportSources(
      const Source& mainSource,
      const Source& currentSource,
      const std::vector<path>& searchPaths,
      std::forward_list<Source>* importedSources,
      std::vector<Diag>* diags);

  auto visit(const parse::ImportStmtNode& n) -> void override;

private:
  const Source& m_mainSource;
  const Source& m_currentSource;
  const std::vector<path>& m_searchPaths;
  std::forward_list<Source>* m_importedSources;
  std::vector<Diag>* m_diags;

  [[nodiscard]] auto import(const path& file, input::Span span) const -> bool;

  [[nodiscard]] auto importFromDir(const path& searchPath, const path& file) const -> bool;
};

} // namespace frontend::internal
