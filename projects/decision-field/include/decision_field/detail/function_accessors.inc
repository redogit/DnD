// Read-only invocation views avoid copying the complete checkpoint for each call.
[[nodiscard]] const ObligationSet& invocation_obligations() const noexcept { return obligations_; }
[[nodiscard]] const InvocationRecord<T>& invocation_record(InvocationId id) const { return invocations_.at(id); }
[[nodiscard]] bool has_transform(std::string_view id) const { return transforms_.contains(std::string(id)); }
