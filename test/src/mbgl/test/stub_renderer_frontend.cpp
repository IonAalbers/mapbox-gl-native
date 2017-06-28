#include <mbgl/test/stub_renderer_frontend.hpp>

#include <mbgl/renderer/renderer.hpp>
#include <mbgl/map/backend_scope.hpp>

namespace mbgl {

StubRendererFrontend::StubRendererFrontend(Backend& backend_, InvalidateCallback invalidate)
    : backend(backend_)
    , asyncInvalidate([this, invalidate=std::move(invalidate)]() {
                invalidate(*this);
            }) {
}

StubRendererFrontend::StubRendererFrontend(Backend& backend_, View& view)
        : backend(backend_)
        , asyncInvalidate([&]() {
                BackendScope guard { backend };
                this->render(view);
            }) {
}

StubRendererFrontend::~StubRendererFrontend() {
    BackendScope guard { backend };
}

void StubRendererFrontend::onLowMemory() {
    if (!renderer) return;
    renderer->onLowMemory();
}

void StubRendererFrontend::dumpDebugLogs() {
    if (!renderer) return;
    renderer->dumpDebugLogs();
}

void StubRendererFrontend::setRenderer(std::unique_ptr<Renderer> renderer_, RendererObserver& observer) {
    renderer = std::move(renderer_);
    renderer->setObserver(&observer);
}

void StubRendererFrontend::update(std::shared_ptr<UpdateParameters> params) {
    updateParameters = std::move(params);
    asyncInvalidate.send();
}

void StubRendererFrontend::render(View& view) {
    assert(BackendScope::exists());
    if (!updateParameters || !renderer) return;

    renderer->render(backend, view, *updateParameters);
}

std::vector<Feature> StubRendererFrontend::queryRenderedFeatures(std::shared_ptr<RenderedQueryParameters> params) const {
    return renderer->queryRenderedFeatures(*params);
}

std::vector<Feature> StubRendererFrontend::querySourceFeatures(std::shared_ptr<SourceQueryParameters> params) const {
    return renderer->querySourceFeatures(*params);
}


} // namespace mbgl

