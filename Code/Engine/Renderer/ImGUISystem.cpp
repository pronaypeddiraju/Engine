//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/ImGUISystem.hpp"
#include "Engine/Renderer/ColorTargetView.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "../../imGUI/imgui.h"
#include "../../imGUI/imgui_internal.h"

//Global pointer for use in Game Systems
ImGUISystem* g_ImGUI = nullptr;
static const float NODE_SLOT_RADIUS = 4.0f;

//------------------------------------------------------------------------------------------------------------------------------
ImGUISystem::ImGUISystem(RenderContext* context)
{
	m_renderContext = context;
	Startup();
}

//------------------------------------------------------------------------------------------------------------------------------
ImGUISystem::~ImGUISystem()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

//------------------------------------------------------------------------------------------------------------------------------
void ImGUISystem::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();
}

//------------------------------------------------------------------------------------------------------------------------------
void ImGUISystem::Render()
{
	ImGui::Render();
	ImDrawData* data = ImGui::GetDrawData();

	//Set RTV for imGUI
	ID3D11RenderTargetView *dx_rtv = m_renderContext->GetFrameColorTarget()->GetAsDXResource();
	ID3D11DeviceContext* context = m_renderContext->GetDXContext();
	context->OMSetRenderTargets(1, &dx_rtv, NULL);

	ImGui_ImplDX11_RenderDrawData(data);
}

//------------------------------------------------------------------------------------------------------------------------------
void ImGUISystem::EndFrame()
{
	ImGui::EndFrame();
}

//------------------------------------------------------------------------------------------------------------------------------
void ImGUISystem::Startup()
{
	//Setup imGUI here
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(m_renderContext->m_hwnd);
	ImGui_ImplDX11_Init(m_renderContext->GetDXDevice(), m_renderContext->GetDXContext());
	ImGui::StyleColorsDark();
	UNUSED(io);
}

//------------------------------------------------------------------------------------------------------------------------------
int ImGUISystem::CurveEditor(const char* label
	, float* values
	, int points_count
	, const ImVec2& editor_size	
	, int* new_count)
{
	enum class StorageValues : ImGuiID
	{
		FROM_X = 100,
		FROM_Y,
		WIDTH,
		HEIGHT,
		IS_PANNING,
		POINT_START_X,
		POINT_START_Y
	};

	const float HEIGHT = 100;
	static ImVec2 start_pan;

	ImGuiContext& g = *ImGui::GetCurrentContext();
	const ImGuiStyle& style = g.Style;
	ImVec2 size = editor_size;
	size.x = size.x < 0 ? ImGui::CalcItemWidth() + (style.FramePadding.x * 2) : size.x;
	size.y = size.y < 0 ? HEIGHT : size.y;

	ImGuiWindow* parent_window = ImGui::GetCurrentWindow();
	ImGuiID id = parent_window->GetID(label);
	if (!ImGui::BeginChildFrame(id, size, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		ImGui::EndChild();
		return -1;
	}

	int hovered_idx = -1;
	if (new_count) *new_count = points_count;

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
	{
		ImGui::EndChild();
		return -1;
	}

	ImVec2 points_min(FLT_MAX, FLT_MAX);
	ImVec2 points_max(-FLT_MAX, -FLT_MAX);
	for (int point_idx = 0; point_idx < points_count; ++point_idx)
	{
		ImVec2 point;
		point = ((ImVec2*)values)[point_idx];

		points_max = ImMax(points_max, point);
		points_min = ImMin(points_min, point);
	}
	points_max.y = ImMax(points_max.y, points_min.y + 0.0001f);

	float from_x = window->StateStorage.GetFloat((ImGuiID)StorageValues::FROM_X, points_min.x);
	float from_y = window->StateStorage.GetFloat((ImGuiID)StorageValues::FROM_Y, points_min.y);
	float width = window->StateStorage.GetFloat((ImGuiID)StorageValues::WIDTH, points_max.x - points_min.x);
	float height = window->StateStorage.GetFloat((ImGuiID)StorageValues::HEIGHT, points_max.y - points_min.y);
	window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_X, from_x);
	window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_Y, from_y);
	window->StateStorage.SetFloat((ImGuiID)StorageValues::WIDTH, width);
	window->StateStorage.SetFloat((ImGuiID)StorageValues::HEIGHT, height);

	ImVec2 beg_pos = ImGui::GetCursorScreenPos();

	const ImRect inner_bb = ImRect(ImGui::GetWindowContentRegionMin(), ImGui::GetWindowContentRegionMax());
	ImVec2 frameMin = inner_bb.Min;
	frameMin.x -= style.FramePadding.x;
	frameMin.y -= style.FramePadding.y;

	ImVec2 frameMax = inner_bb.Max;
	frameMin.x += style.FramePadding.x;
	frameMin.y += style.FramePadding.y;

	const ImRect frame_bb(frameMin, frameMax);

	auto transform = [&](const ImVec2& pos) -> ImVec2
	{
		float x = (pos.x - from_x) / width;
		float y = (pos.y - from_y) / height;

		return ImVec2(
			inner_bb.Min.x * (1 - x) + inner_bb.Max.x * x,
			inner_bb.Min.y * y + inner_bb.Max.y * (1 - y)
		);
	};

	auto invTransform = [&](const ImVec2& pos) -> ImVec2
	{
		float x = (pos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x);
		float y = (inner_bb.Max.y - pos.y) / (inner_bb.Max.y - inner_bb.Min.y);

		return ImVec2(
			from_x + width * x,
			from_y + height * y
		);
	};

	int exp;
	frexp(width / 5, &exp);
	float step_x = (float)ldexp(1.0, exp);
	int cell_cols = int(width / step_x);

	float x = step_x * int(from_x / step_x);
	for (int i = -1; i < cell_cols + 2; ++i)
	{
		ImVec2 a = transform({ x + i * step_x, from_y });
		ImVec2 b = transform({ x + i * step_x, from_y + height });
		window->DrawList->AddLine(a, b, 0x55000000);
		char buf[64];
		if (exp > 0)
		{
			ImFormatString(buf, sizeof(buf), " %d", int(x + i * step_x));
		}
		else
		{
			ImFormatString(buf, sizeof(buf), " %f", x + i * step_x);
		}
		window->DrawList->AddText(b, 0x55000000, buf);
	}

	frexp(height / 5, &exp);
	float step_y = (float)ldexp(1.0, exp);
	int cell_rows = int(height / step_y);

	float y = step_y * int(from_y / step_y);
	for (int i = -1; i < cell_rows + 2; ++i)
	{
		ImVec2 a = transform({ from_x, y + i * step_y });
		ImVec2 b = transform({ from_x + width, y + i * step_y });
		window->DrawList->AddLine(a, b, 0x55000000);
		char buf[64];
		if (exp > 0)
		{
			ImFormatString(buf, sizeof(buf), " %d", int(y + i * step_y));
		}
		else
		{
			ImFormatString(buf, sizeof(buf), " %f", y + i * step_y);
		}
		window->DrawList->AddText(a, 0x55000000, buf);
	}

	if (ImGui::GetIO().MouseWheel != 0 && ImGui::IsItemHovered())
	{
		float scale = powf(2, ImGui::GetIO().MouseWheel);
		width *= scale;
		height *= scale;
		window->StateStorage.SetFloat((ImGuiID)StorageValues::WIDTH, width);
		window->StateStorage.SetFloat((ImGuiID)StorageValues::HEIGHT, height);
	}
	if (ImGui::IsMouseReleased(1))
	{
		window->StateStorage.SetBool((ImGuiID)StorageValues::IS_PANNING, false);
	}
	if (window->StateStorage.GetBool((ImGuiID)StorageValues::IS_PANNING, false))
	{
		ImVec2 drag_offset = ImGui::GetMouseDragDelta(1);
		from_x = start_pan.x;
		from_y = start_pan.y;
		from_x -= drag_offset.x * width / (inner_bb.Max.x - inner_bb.Min.x);
		from_y += drag_offset.y * height / (inner_bb.Max.y - inner_bb.Min.y);
		window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_X, from_x);
		window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_Y, from_y);
	}
	else if (ImGui::IsMouseDragging(1) && ImGui::IsItemHovered())
	{
		window->StateStorage.SetBool((ImGuiID)StorageValues::IS_PANNING, true);
		start_pan.x = from_x;
		start_pan.y = from_y;
	}

	int changed_idx = -1;
	for (int point_idx = points_count - 2; point_idx >= 0; --point_idx)
	{
		ImVec2* points;
		points = ((ImVec2*)values) + point_idx;

		ImVec2 p_prev = points[0];
		ImVec2 tangent_last;
		ImVec2 tangent;
		ImVec2 p;

		p = points[1];

		auto handlePoint = [&](ImVec2& p, int idx) -> bool
		{
			static const float SIZE = 3;

			ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
			ImVec2 pos = transform(p);
			pos.x -= SIZE;
			pos.y -= SIZE;

			ImGui::SetCursorScreenPos(pos);
			ImGui::PushID(idx);
			ImGui::InvisibleButton("", ImVec2(2 * NODE_SLOT_RADIUS, 2 * NODE_SLOT_RADIUS));

			ImU32 col = ImGui::IsItemActive() || ImGui::IsItemHovered() ? ImGui::GetColorU32(ImGuiCol_PlotLinesHovered) : ImGui::GetColorU32(ImGuiCol_PlotLines);

			window->DrawList->AddLine(ImVec2(pos.x - SIZE, pos.y), ImVec2(pos.x, pos.y + SIZE), col);
			window->DrawList->AddLine(ImVec2(pos.x + SIZE, pos.y), ImVec2(pos.x, pos.y + SIZE), col);
			window->DrawList->AddLine(ImVec2(pos.x + SIZE, pos.y), ImVec2(pos.x, pos.y - SIZE), col);
			window->DrawList->AddLine(ImVec2(pos.x - SIZE, pos.y), ImVec2(pos.x, pos.y - SIZE), col);

			if (ImGui::IsItemHovered()) hovered_idx = point_idx + idx;

			bool changed = false;
			if (ImGui::IsItemActive() && ImGui::IsMouseClicked(0))
			{
				window->StateStorage.SetFloat((ImGuiID)StorageValues::POINT_START_X, pos.x);
				window->StateStorage.SetFloat((ImGuiID)StorageValues::POINT_START_Y, pos.y);
			}

			if (ImGui::IsItemHovered() || ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
			{
				char tmp[64];
				ImFormatString(tmp, sizeof(tmp), "%0.2f, %0.2f", p.x, p.y);
				window->DrawList->AddText({ pos.x, pos.y - ImGui::GetTextLineHeight() }, 0xff000000, tmp);
			}

			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
			{
				pos.x = window->StateStorage.GetFloat((ImGuiID)StorageValues::POINT_START_X, pos.x);
				pos.y = window->StateStorage.GetFloat((ImGuiID)StorageValues::POINT_START_Y, pos.y);
				pos.x += ImGui::GetMouseDragDelta().x;
				pos.y += ImGui::GetMouseDragDelta().y;
				ImVec2 v = invTransform(pos);

				p = v;
				changed = true;
			}
			ImGui::PopID();

			ImGui::SetCursorScreenPos(cursor_pos);
			return changed;
		};

		auto handleTangent = [&](ImVec2& t, const ImVec2& p, int idx) -> bool
		{
			static const float SIZE = 2;
			static const float LENGTH = 18;

			auto normalized = [](const ImVec2& v) -> ImVec2
			{
				float len = 1.0f / sqrtf(v.x *v.x + v.y * v.y);
				return ImVec2(v.x * len, v.y * len);
			};

			ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
			ImVec2 pos = transform(p);
			ImVec2 tang = pos;
			tang.x += normalized(ImVec2(t.x, -t.y)).x;
			tang.y += normalized(ImVec2(t.x, -t.y)).y;
			tang.x *= LENGTH;
			tang.y *= LENGTH;

			ImVec2 cursorScreenPos = tang;
			cursorScreenPos.x -= SIZE;
			cursorScreenPos.y -= SIZE;

			ImGui::SetCursorScreenPos(cursorScreenPos);
			ImGui::PushID(-idx);
			ImGui::InvisibleButton("", ImVec2(2 * NODE_SLOT_RADIUS, 2 * NODE_SLOT_RADIUS));

			window->DrawList->AddLine(pos, tang, ImGui::GetColorU32(ImGuiCol_PlotLines));

			ImU32 col = ImGui::IsItemHovered() ? ImGui::GetColorU32(ImGuiCol_PlotLinesHovered) : ImGui::GetColorU32(ImGuiCol_PlotLines);

			window->DrawList->AddLine(ImVec2(tang.x - SIZE, tang.y + SIZE), ImVec2(tang.x + SIZE, tang.y + SIZE), col);
			window->DrawList->AddLine(ImVec2(tang.x + SIZE, tang.y + SIZE), ImVec2(tang.x + SIZE, tang.y - SIZE), col);
			window->DrawList->AddLine(ImVec2(tang.x + SIZE, tang.y - SIZE), ImVec2(tang.x - SIZE, tang.y - SIZE), col);
			window->DrawList->AddLine(ImVec2(tang.x - SIZE, tang.y - SIZE), ImVec2(tang.x - SIZE, tang.y + SIZE), col);

			bool changed = false;
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
			{
				tang = ImGui::GetIO().MousePos;
				tang.x -= pos.x;
				tang.y -= pos.y;
				tang = normalized(tang);
				tang.y *= -1;

				t = tang;
				changed = true;
			}
			ImGui::PopID();

			ImGui::SetCursorScreenPos(cursor_pos);
			return changed;
		};

		ImGui::PushID(point_idx);
		
		window->DrawList->AddBezierCurve(
			transform(p_prev),
			transform(ImVec2(p_prev.x + tangent_last.x, p_prev.y + tangent_last.y)),
			transform(ImVec2(p.x + tangent.x, p.y + tangent.y)),
			transform(p),
			ImGui::GetColorU32(ImGuiCol_PlotLines),
			1.0f,
			20);
		if (handleTangent(tangent_last, p_prev, 0))
		{
			points[1] = ImClamp(tangent_last, ImVec2(0, -1), ImVec2(1, 1));
			changed_idx = point_idx;
		}
		if (handleTangent(tangent, p, 1))
		{
			points[2] = ImClamp(tangent, ImVec2(-1, -1), ImVec2(0, 1));
			changed_idx = point_idx + 1;
		}
		if (handlePoint(p, 1))
		{
			if (p.x <= p_prev.x) p.x = p_prev.x + 0.001f;
			if (point_idx < points_count - 2 && p.x >= points[6].x)
			{
				p.x = points[6].x - 0.001f;
			}
			points[3] = p;
			changed_idx = point_idx + 1;
		}

		if (point_idx == 0)
		{
			if (handlePoint(p_prev, 0))
			{
				if (p.x <= p_prev.x) p_prev.x = p.x - 0.001f;
				points[0] = p_prev;
				changed_idx = point_idx;
			}
		}
		ImGui::PopID();
	}

	ImGui::SetCursorScreenPos(inner_bb.Min);

	ImGui::InvisibleButton("bg", ImVec2(inner_bb.Max.x - inner_bb.Min.x, inner_bb.Max.y - inner_bb.Min.y));

	if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0) && new_count)
	{
		ImVec2 mp = ImGui::GetMousePos();
		ImVec2 new_p = invTransform(mp);
		ImVec2* points = (ImVec2*)values;

		points[points_count * 3 + 0] = ImVec2(-0.2f, 0);
		points[points_count * 3 + 1] = new_p;
		points[points_count * 3 + 2] = ImVec2(0.2f, 0);;
		++*new_count;

		auto compare = [](const void* a, const void* b) -> int
		{
			float fa = (((const ImVec2*)a) + 1)->x;
			float fb = (((const ImVec2*)b) + 1)->x;
			return fa < fb ? -1 : (fa > fb) ? 1 : 0;
		};

		qsort(values, points_count + 1, sizeof(ImVec2) * 3, compare);
	}

	if (hovered_idx >= 0 && ImGui::IsMouseDoubleClicked(0) && new_count && points_count > 2)
	{
		ImVec2* points = (ImVec2*)values;
		--*new_count;
		for (int j = hovered_idx * 3; j < points_count * 3 - 3; j += 3)
		{
			points[j + 0] = points[j + 3];
			points[j + 1] = points[j + 4];
			points[j + 2] = points[j + 5];
		}
	}

	ImGui::EndChildFrame();
	ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
	return changed_idx;
}