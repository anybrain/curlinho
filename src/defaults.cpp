/*
* Created by José Luis Silva on 2019-09-06.
* Copyright (c) 2019 Deeep. All rights reserved.
*/

#include "curlinho/defaults.h"
#include "curlinho/hmac.h"

namespace curlinho {
void Defaults::SetUrl(const Url &url) { url_ = url; }
void Defaults::SetHeaders(const Headers &headers) { headers_ = headers; }
void Defaults::SetTimeout(const Timeout &timeout) { timeout_ = timeout; }
void Defaults::SetAuth(const Authentication &auth) { auth_ = auth; }
void Defaults::SetProtocolVersion(
    const ProtocolVersion &protocolVersion) { protocolVersion_ = protocolVersion; }
void Defaults::SetRetryPolicy(const RetryPolicy &retryPolicy) {
  retryPolicy_ = retryPolicy;
}
void Defaults::SetHmacAuth(const curlinho::Hmac &hmac){ hmac_ = hmac;}

void Defaults::SetOption(const Url &url) { Defaults::SetUrl(url); }
void Defaults::SetOption(const Headers &headers) { Defaults::SetHeaders(headers); }
void Defaults::SetOption(const Timeout &timeout) { Defaults::SetTimeout(timeout); }
void Defaults::SetOption(const Authentication &auth) { Defaults::SetAuth(auth); }
void Defaults::SetOption(const ProtocolVersion &protocolVersion) {
  Defaults::SetProtocolVersion(protocolVersion);
}
void Defaults::SetOption(const RetryPolicy &retryPolicy) {
  Defaults::SetRetryPolicy(retryPolicy);
}
void Defaults::SetOption(const Hmac &hmac) {
  Defaults::SetHmacAuth(hmac);
}

bool Defaults::HasUrl() { return !url_.empty(); }
bool Defaults::HasHeaders() { return !headers_.empty(); }
bool Defaults::HasTimeout() { return timeout_.Milliseconds()!= 0; }
bool Defaults::HasAuth() { return !auth_.empty(); }
bool Defaults::HasHmac() { return !hmac_.empty(); }

Hmac Defaults::GetHmacAuth() { return hmac_;}
}