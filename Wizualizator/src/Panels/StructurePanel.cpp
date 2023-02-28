#include "Panels/StructurePanel.hpp"
#include "Camera.hpp"
#include "Canvas.hpp"
#include "Event.hpp"
#include "GUI.hpp"
#include <imgui.h>

StructurePanel::StructurePanel() noexcept
	:
	PanelBase("Structure Panel"),
	m_Camera(std::make_unique<Camera>(16.0f / 9.0f, 1.0f)),
	m_Canvas(std::make_unique<Canvas>(1280U, 960U, 1.0f, 1.0f, 1.0f)),
	m_IsPanelHovered(false),
	m_IsPanelDragged(false),
	m_ShouldRedraw(true), /* Clear image at startup */
	m_PHLevel(7.0f)
{
	Project::SubscribeContextSelection([this]() 
	{ 
		m_ShouldRedraw = true;							// Signal a redraw is needed
		m_Camera->SetPosition(Vector2{ 0.0f, 0.0f });	// Reset camera position
		m_Camera->SetScale(1.0f);
	});
}

void StructurePanel::OnGUIRender()
{
	constexpr ImGuiWindowFlags windowFlags
	{
		ImGuiWindowFlags_NoScrollbar		|
		ImGuiWindowFlags_NoScrollWithMouse	|
		ImGuiWindowFlags_None
	};

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::PushStyleColor(ImGuiCol_WindowBg, { 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::Begin(GetPanelName().data(), nullptr, windowFlags);
	ImGui::PopStyleColor();

	m_IsPanelHovered = ImGui::IsWindowHovered();
	m_IsPanelDragged = ImGui::IsMouseDown(ImGuiMouseButton_Left);
	const Vector2 windowSize{ ImGui::GetWindowSize().x, ImGui::GetWindowSize().y };

	ImGui::PopStyleVar();
	ImGui::AlignTextToFramePadding();

	m_Camera->SetAspectRatio(windowSize.x / windowSize.y);
	if (m_Canvas->SetSize(static_cast<uint32_t>(windowSize.x), static_cast<uint32_t>(windowSize.y)) || m_ShouldRedraw)
		RedrawCanvas();

	if (Project::OnSequenceSelected())
	{
		if (Project::SelectedSequenceType() == Project::ESequenceSelectionType::NucleotideSequence && !Project::SelectedPeptide())
		{
			constexpr std::string_view prompt{ "No open reading frame selected" };
			ImGui::SetCursorPos(ImVec2{ windowSize.width * 0.5f - (ImGui::CalcTextSize(prompt.data()).x * 0.5f), windowSize.height * 0.5f });
			ImGui::PushStyleColor(ImGuiCol_Text, { 0.0f, 0.0f, 0.0f, 1.0f });
			ImGui::Text(prompt.data());
			ImGui::PopStyleColor();
		}
		else
		{
			const auto cursorScreenPosition{ ImGui::GetCursorScreenPos() };
			ImGui::Image(reinterpret_cast<ImTextureID>(m_Canvas->GetImage()), { windowSize.width, windowSize.height }, { 0.0f, 1.0f }, { 1.0f, 0.0f });

			ImGui::SetNextWindowPos({ cursorScreenPosition.x + 10.0f, cursorScreenPosition.y + 10.0f });
			ImGui::BeginChild("PH Modifier Window", { 200.0f, 50.0f });
			m_ShouldRedraw |= ImGui::DragFloat("PH", &m_PHLevel, 0.1f, 0.1f, 14.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::EndChild();
		}
	}
	else
	{
		constexpr std::string_view prompt{ "No selected sequence to visualize" };
		ImGui::SetCursorPos(ImVec2{ windowSize.width * 0.5f - (ImGui::CalcTextSize(prompt.data()).x * 0.5f), windowSize.height * 0.5f });
		ImGui::PushStyleColor(ImGuiCol_Text, { 0.0f, 0.0f, 0.0f, 1.0f });
		ImGui::Text(prompt.data());
		ImGui::PopStyleColor();
	}

	ImGui::End();
}

void StructurePanel::OnEvent(const Event& event)
{
	switch (event.GetEventType())
	{
		case EEventType::MouseScrolled:
		{
			m_Camera->OnEvent(event, m_IsPanelHovered);

			if (m_IsPanelHovered)
				m_ShouldRedraw = true;
		} break;

		case EEventType::MouseCursorMoved:
		{
			const MouseCursorMovedEvent& mouseScrolledEvent{ EventCast<MouseCursorMovedEvent>(event) };
			const bool shouldUpdate{ m_IsPanelHovered && m_IsPanelDragged };
			m_Camera->OnEvent(mouseScrolledEvent, shouldUpdate);

			if (shouldUpdate)
				m_ShouldRedraw = true;
		} break;

		default: break;
	}
}

struct LineRenderer
{
	float X{ 0.0f }, Y{ 0.0f }, Rotation{ 0.0f };
};

struct SplitLineRenderer
{
	float X{ 0.0f }, Y{ 0.0f }, Rotation{ 0.0f };
};

struct StrippedLineRenderer
{
	float X{ 0.0f }, Y{ 0.0f }, Rotation{ 0.0f };
};

struct TriangleRenderer
{
	float X{ 0.0f }, Y{ 0.0f }, Rotation{ 0.0f };
	bool Stripped{ false };
};

struct TextRenderer
{
	float X{ 0.0f }, Y{ 0.0f }, Scale{ 1.0f };
	std::string Text{};
};

static void DrawLine(const std::unique_ptr<Canvas>& canvas, const Vector2 positionParam, const float rotation = 0.0f, const float scale = 1.0f)
{
	Vector2 position{ positionParam };
	canvas->DrawLine({ position.x, position.y }, { position.x + (0.1f * scale), position.y }, rotation);
}

static void DrawSplitLine(const std::unique_ptr<Canvas>& canvas, const Vector2 position, const float rotation = 0.0f)
{
	canvas->DrawLine({ position.x + 0.005f , position.y - 0.05f }, { position.x + 0.005f, position.y + 0.05f }, rotation);
	canvas->DrawLine({ position.x - 0.005f , position.y - 0.05f }, { position.x - 0.005f, position.y + 0.05f }, rotation);
}

static void DrawStrippedLine(const std::unique_ptr<Canvas>& canvas, const Vector2 positionParam, [[maybe_unused]] const float rotation = 0.0f)
{
	DrawLine(canvas, positionParam, rotation);
}

static void DrawTriangle(const std::unique_ptr<Canvas>& canvas, const Vector2 position, const float rotation, bool stripped)
{
	if (stripped)
	{
		float startWidth{ 0.0022f };

		canvas->DrawLine({ position.x - startWidth, position.y - 0.016f }, { position.x + startWidth, position.y - 0.016f }, rotation, position);
		startWidth *= 1.55f;
		canvas->DrawLine({ position.x - startWidth, position.y - 0.032f }, { position.x + startWidth, position.y - 0.032f }, rotation, position);
		startWidth *= 1.5f;
		canvas->DrawLine({ position.x - startWidth, position.y - 0.048f }, { position.x + startWidth, position.y - 0.048f }, rotation, position);
		startWidth *= 1.35f;
		canvas->DrawLine({ position.x - startWidth, position.y - 0.064f }, { position.x + startWidth, position.y - 0.064f }, rotation, position);
		startWidth *= 1.25f;
		canvas->DrawLine({ position.x - startWidth, position.y - 0.080f }, { position.x + startWidth, position.y - 0.080f }, rotation, position);
		startWidth *= 1.25f;
		canvas->DrawLine({ position.x - startWidth, position.y - 0.096f }, { position.x + startWidth, position.y - 0.096f }, rotation, position);
	}
	else
	{
		const Vector2 pivot{ position.x, position.y };

		canvas->DrawLine({ position.x + 0.01f, position.y - 0.1f }, { position.x , position.y }, rotation, pivot);
		canvas->DrawLine({ position.x - 0.01f, position.y - 0.1f }, { position.x , position.y }, rotation, pivot);
		canvas->DrawLine({ position.x + 0.01f , position.y - 0.1f }, { position.x - 0.01f , position.y - 0.1f }, rotation, pivot);
	}
}

struct AminoDrawList
{
	std::vector<LineRenderer>			Lines;
	std::vector<SplitLineRenderer>		SplitLines;
	std::vector<StrippedLineRenderer>	StrippedLines;
	std::vector<TriangleRenderer>		Triangles;
	std::vector<TextRenderer>			Texts;

	void Draw(std::unique_ptr<Canvas>& canvas, const Vector2 cursorPosition, const bool flip) const
	{
		for (auto& line : Lines)
			DrawLine(canvas, { cursorPosition.x + line.X, flip ? -line.Y + cursorPosition.y : line.Y + cursorPosition.y }, flip ? 360.0f - line.Rotation + 180.0f : line.Rotation);

		for (auto& splitLine : SplitLines)
			DrawSplitLine(canvas, { splitLine.X + cursorPosition.x, flip ? -splitLine.Y + cursorPosition.y : splitLine.Y + cursorPosition.y }, flip ? 360.0f - splitLine.Rotation + 180.0f : splitLine.Rotation);

		for (auto& strippedLine : StrippedLines)
			DrawStrippedLine(canvas, { cursorPosition.x + strippedLine.X, flip ? cursorPosition.y - strippedLine.Y : cursorPosition.y + strippedLine.Y }, flip ? 360.0f - strippedLine.Rotation + 180.0f : strippedLine.Rotation);

		for (auto& triangle : Triangles)
			DrawTriangle(canvas, { cursorPosition.x + triangle.X, flip ? -triangle.Y + cursorPosition.y : triangle.Y + cursorPosition.y }, flip ? 360.0f - triangle.Rotation + 180.0f : triangle.Rotation, flip ? !triangle.Stripped : triangle.Stripped);

		for (auto& text : Texts)
		{
			if (flip)
			{
				const float yPosition{ -text.Y - 0.0325f };
				canvas->DrawText(text.Text, { cursorPosition.x + text.X - 0.003f, cursorPosition.y + yPosition }, 1.0f);
			}
			else
				canvas->DrawText(text.Text, { cursorPosition.x + text.X - 0.003f, cursorPosition.y + text.Y }, 1.0f);
		}
	}
};

struct AminoDrawRange
{
	float MinPH;
	float MaxPH;
	AminoDrawList DrawList;

	bool MeetsPH(const float pH) const noexcept
	{
		return pH >= MinPH && pH <= MaxPH;
	}
};

#define TEXT_TO_LITERAL(text) #text
static std::unordered_map<char, std::vector<AminoDrawRange>> s_DrawLists
{
	{
		'R',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					12.40f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.19700001, -0.389, 90 },
							LineRenderer{ 0.16399999, -0.312, 325 },
							LineRenderer{ 0.123, -0.234, 90 },
							LineRenderer{ 0.082, -0.156, 325 },
							LineRenderer{ 0.0405, -0.078, 90 },
							LineRenderer{ 0.163, -1.86265e-09, 325 },
							LineRenderer{ 0.081, -1.86265e-09, 35 },
							LineRenderer{ 0.269, -0.491, 145 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{
							StrippedLineRenderer{ 0.136, -0.501, 35 },
							StrippedLineRenderer{ 0.206, -0.389, 90 },
							StrippedLineRenderer{ 0.134, -0.491, 35 },
							StrippedLineRenderer{ 0.267, -0.501, 145 }
						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.229, -0.341, 0.8, TEXT_TO_LITERAL(NH) },
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) },
							TextRenderer{ 0.224 + 0.003, -0.47, 1, TEXT_TO_LITERAL((+)) },
							TextRenderer{ 0.067999996 , -0.549, 0.8, TEXT_TO_LITERAL(H2N) },
							TextRenderer{ 0.35599998, -0.549, 0.8, TEXT_TO_LITERAL(NH2) }
						}
					},
				},
				{
					12.50f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.16300002, -0.46100003, 35 },
							LineRenderer{ 0.16300002, -0.47000003, 35 },
							LineRenderer{ 0.24499999, -0.46600005, 325 },
							LineRenderer{ 0.2035, -0.39100003, 90 },
							LineRenderer{ 0.163, -0.31300002, 325 },
							LineRenderer{ 0.121999994, -0.23500001, 90 },
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.164, 0, 325 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.333, -0.525, 1, TEXT_TO_LITERAL(NH2) },
							TextRenderer{ 0.10300001, -0.52500004, 1, TEXT_TO_LITERAL(HN) },
							TextRenderer{ 0.23600002, -0.34400004, 1, TEXT_TO_LITERAL(NH) },
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
			}
		}
	},
	{
		'A',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0, 0, 325 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.028, 0, true }
						},
						.Texts
						{
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
			}
		}
	},
	{
		'N',
		{
			std::vector<AminoDrawRange>
			{
				{
                     0.00f,
                     14.00f,
                     AminoDrawList
                     {
                          .Lines
                          {
                              LineRenderer{ 0.173, -0.186, 0 },
                              LineRenderer{ 0.082, -0.157, 145 },
                              LineRenderer{ 0.041, -0.078, 90 },
                              LineRenderer{ 0.082, 0.001, 35 },
                              LineRenderer{ 0.164, 0.001, 325 }
                          },
                          .SplitLines
                          {
                              SplitLineRenderer{ 0.173, -0.236, 0 },
							  SplitLineRenderer{ 0.173, 0.078, 0 }
                          },
                          .StrippedLines
                          {

                          },
                          .Triangles
                          {
                              TriangleRenderer{ 0.091, -0.029, 235, true }
                          },
                          .Texts
						  {
                              TextRenderer{ 0.158, -0.316, 0.8, TEXT_TO_LITERAL(O) },
                              TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) },
                              TextRenderer{ 0.273, -0.2, 0.8, TEXT_TO_LITERAL(NH2) }
						  }
					},
                },
			}
		}
	},
	{
		'D',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					3.80f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.173, -0.187, 0 },
							LineRenderer{ 0.082, -0.158, 325 },
							LineRenderer{ 0.041, -0.079, 90 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.164, 0, 325 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, -0.237, 0 },
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.275, -0.2, 0.8, TEXT_TO_LITERAL(OH) },
							TextRenderer{ 0.158, -0.318, 0.8, TEXT_TO_LITERAL(O) },
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
				{
					3.90f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.041, -0.079, 90 },
							LineRenderer{ 0.082, -0.158, 325 },
							LineRenderer{ 0.173, -0.187, 0 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 },
							SplitLineRenderer{ 0.173, -0.237, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) },
							TextRenderer{ 0.158, -0.318, 0.8, TEXT_TO_LITERAL(O) },
							TextRenderer{ 0.275, -0.2, 0.8, TEXT_TO_LITERAL(O-) }
						}
					},
				},
			}
		}
	},
	{
		'C',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					8.20f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.082, -0.158, 325 },
							LineRenderer{ 0.041, -0.079, 90 },
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.082, 0, 35 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.172, -0.222, 1, TEXT_TO_LITERAL(SH) },
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
				{
					8.30f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.082, -0.158, 325 },
							LineRenderer{ 0.041, -0.079, 90 },
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.082, 0, 35 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.172, -0.222, 1, TEXT_TO_LITERAL(S-) },
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
			}
		}
	},
	{
		'Q',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.164, -0.314, 325 },
							LineRenderer{ 0.123, -0.235, 90 },
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.164, 0, 325 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.133, -0.314, 125 },
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.062, -0.369, 0.8, TEXT_TO_LITERAL(O) },
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) },
							TextRenderer{ 0.255, -0.372, 0.8, TEXT_TO_LITERAL(NH2) }
						}
					},
				},
			}
		}
	},
	{
		'E',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					4.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.164, -0.314, 325 },
							LineRenderer{ 0.123, -0.235, 90 },
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.164, 0, 325 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.133, -0.314, 125 },
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.062, -0.369, 0.8, TEXT_TO_LITERAL(O) },
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) },
							TextRenderer{ 0.255, -0.372, 0.8, TEXT_TO_LITERAL(OH) }
						}
					},
				},
				{
					4.10f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.164, -0.314, 325 },
							LineRenderer{ 0.123, -0.235, 90 },
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.164, 0, 325 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.133, -0.314, 125 },
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.062, -0.369, 0.8, TEXT_TO_LITERAL(O) },
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) },
							TextRenderer{ 0.255, -0.372, 0.8, TEXT_TO_LITERAL(O-) }
						}
					},
				}
			}
		}
	},
	{
		'G',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.001, 0, 325 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{

						},
						.Texts
						{
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
			}
		}
	},
	{
		'H',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					6.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.107, -0.233, 72 },
							LineRenderer{ 0.132, -0.31, 324 },
							LineRenderer{ 0.213, -0.31, 216 },
							LineRenderer{ 0.238, -0.233, 108 },
							LineRenderer{ 0.173, -0.186, 0 },
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, -0.157, 325 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.209, -0.27, 1, TEXT_TO_LITERAL(+) },
							TextRenderer{ 0.208, -0.38, 0.8, TEXT_TO_LITERAL(H) },
							TextRenderer{ 0.208, -0.345, 0.8, TEXT_TO_LITERAL(N) },
							TextRenderer{ 0.244, -0.199, 0.8, TEXT_TO_LITERAL(NHJ) },
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
				{
					6.10f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.107, -0.233, 72 },
							LineRenderer{ 0.132, -0.31, 324 },
							LineRenderer{ 0.213, -0.31, 216 },
							LineRenderer{ 0.238, -0.233, 108 },
							LineRenderer{ 0.173, -0.186, 0 },
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 0.122, -0.238, 72 },
							LineRenderer{ 0.223, -0.238, 108 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.208, -0.38, 0.8, TEXT_TO_LITERAL(H) },
							TextRenderer{ 0.208, -0.345, 0.8, TEXT_TO_LITERAL(N) },
							TextRenderer{ 0.258, -0.199, 0.8, TEXT_TO_LITERAL(N) },
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
			}
		}
	},
	{
		'I',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.123, -0.235, 90 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true },
							TriangleRenderer{ 0.091, -0.128, 305, true }
						},
						.Texts
						{
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
			}
		}
	},
	{
		'L',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.173, -0.186, 0 },
							LineRenderer{ 0.123, -0.236, 90 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
			}
		}
	},
	{
		'K',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					10.50f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.205, -0.394, 90 },
							LineRenderer{ 0.164, -0.315, 325 },
							LineRenderer{ 0.123, -0.236, 90 },
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.164, 0, 325 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.219, -0.473, 0.8, TEXT_TO_LITERAL(+NH3) },
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
				{
					10.60f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.205, -0.394, 90 },
							LineRenderer{ 0.164, -0.315, 325 },
							LineRenderer{ 0.123, -0.236, 90 },
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.164, 0, 325 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.241, -0.473, 0.8, TEXT_TO_LITERAL(NH2) },
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				}
			}
		}
	},
	{
		'M',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 0.123, -0.236, 90 },
							LineRenderer{ 0.164, -0.343, 325 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) },
							TextRenderer{ 0.163, -0.314, 0.8, TEXT_TO_LITERAL(S) }
						}
					},
				},
			}
		}
	},
	{
		'F',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 0.123, -0.236, 90 },
							LineRenderer{ 0.166, -0.161, 30 },
							LineRenderer{ 0.252, -0.161, 150 },
							LineRenderer{ 0.295, -0.236, 270 },
							LineRenderer{ 0.252, -0.311, 30 },
							LineRenderer{ 0.166, -0.311, 150 },
							LineRenderer{ 0.138, -0.236, 90 },
							LineRenderer{ 0.244, -0.174, 150 },
							LineRenderer{ 0.245, -0.298, 30 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
			}
		}
	},
	{
		'P',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.041, -0.079, 90 },
							LineRenderer{ -0.008, -0.138, 10 },
							LineRenderer{ -0.082, -0.103, 120 },
							LineRenderer{ -0.082, -0.017, 60 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
			}
		}
	},
	{
		'S',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, -0.157, 325 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) },
							TextRenderer{ 0.171, -0.212, 0.8, TEXT_TO_LITERAL(OH) }
						}
					},
				},
			}
		}
	},
	{
		'T',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 9.31323e-10, -0.157, 35 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.171, -0.212, 0.8, TEXT_TO_LITERAL(OH) },
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
			}
		}
	},
	{
		'W',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 0.172, -0.178, 9 },
							LineRenderer{ 0.115, -0.235, 81 },
							LineRenderer{ 0.244, -0.215, 117 },
							LineRenderer{ 0.231, -0.295, 225 },
							LineRenderer{ 0.151, -0.307, 153 },
							LineRenderer{ 0.174, -0.191, 9 },
							LineRenderer{ 0.065, -0.312, 213 },
							LineRenderer{ 0.026, -0.389, 273 },
							LineRenderer{ 0.073, -0.461, 153 },
							LineRenderer{ 0.159, -0.457, 213 },
							LineRenderer{ 0.198, -0.38, 93 },
							LineRenderer{ 0.041, -0.388, 273 },
							LineRenderer{ 0.145, -0.32, 153 },
							LineRenderer{ 0.151, -0.444, 213 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) },
							TextRenderer{ 0.298, -0.278, 1, TEXT_TO_LITERAL(NH) }
						}
					},
				},
			}
		}	
	},
	{
		'Y',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					10.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.336, -0.315, 325 },
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 0.123, -0.236, 90 },
							LineRenderer{ 0.166, -0.161, 30 },
							LineRenderer{ 0.252, -0.161, 150 },
							LineRenderer{ 0.295, -0.236, 270 },
							LineRenderer{ 0.252, -0.311, 30 },
							LineRenderer{ 0.166, -0.311, 150 },
							LineRenderer{ 0.138, -0.236, 90 },
							LineRenderer{ 0.244, -0.174, 150 },
							LineRenderer{ 0.245, -0.298, 30 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.425, -0.37, 0.8, TEXT_TO_LITERAL(OH) },
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
				{
					10.10f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.336, -0.315, 325 },
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 0.123, -0.236, 90 },
							LineRenderer{ 0.166, -0.161, 30 },
							LineRenderer{ 0.252, -0.161, 150 },
							LineRenderer{ 0.295, -0.236, 270 },
							LineRenderer{ 0.252, -0.311, 30 },
							LineRenderer{ 0.166, -0.311, 150 },
							LineRenderer{ 0.138, -0.236, 90 },
							LineRenderer{ 0.244, -0.174, 150 },
							LineRenderer{ 0.245, -0.298, 30 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.425, -0.37, 0.8, TEXT_TO_LITERAL(O - ) },
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
			}
		}
	},
	{
		'V',
		{
			std::vector<AminoDrawRange>
			{
				{
					0.00f,
					14.00f,
					AminoDrawList
					{
						.Lines
						{
							LineRenderer{ 0.164, 0, 325 },
							LineRenderer{ 0.082, 0, 35 },
							LineRenderer{ 0.041, -0.078, 90 },
							LineRenderer{ 0.082, -0.157, 325 },
							LineRenderer{ 6.98492e-10, -0.157, 35 }
						},
						.SplitLines
						{
							SplitLineRenderer{ 0.173, 0.078, 0 }
						},
						.StrippedLines
						{

						},
						.Triangles
						{
							TriangleRenderer{ 0.091, -0.029, 235, true }
						},
						.Texts
						{
							TextRenderer{ 0.158, 0.131, 0.8, TEXT_TO_LITERAL(O) }
						}
					},
				},
			}
		}
	}
};

void StructurePanel::RedrawCanvas()
{
	static std::string f_Empty;
	static std::string& f_ReferencedBuffer = f_Empty;

	f_Empty.clear();
	if(!Project::OnSequenceSelected
		(
			[this]([[maybe_unused]] const Project::NucleotideSequenceCache&)
			{},
			[this](const Project::NucleotideSequencePeptideCache& nucleotideSequencePeptideCache)
			{
				f_ReferencedBuffer = nucleotideSequencePeptideCache.ProteinCandidate;
			},
			[this](const Project::AminoSequenceCache& aminoSequenceCache)
			{
				f_ReferencedBuffer = aminoSequenceCache.AminoSequence;
			},
			[this](const Project::AminoSequencePeptideCache& aminoSequencePeptideCache)
			{
				f_ReferencedBuffer = aminoSequencePeptideCache.ProteinCandidate;
			}
		)
	)
	{
		f_ReferencedBuffer.clear();
	}

	f_ReferencedBuffer.erase(std::remove(f_ReferencedBuffer.begin(), f_ReferencedBuffer.end(), '-'), f_ReferencedBuffer.end());
	m_Canvas->BeginDraw(m_Camera);
	{
		bool flip{ false };
		Vector2 cursorPosition{ 0.0f, 0.0f };

		if (!f_ReferencedBuffer.empty())
		{
			if (m_PHLevel <= 9.8f)
			{
				m_Canvas->DrawText("H N+H", { cursorPosition.x - 0.15f, cursorPosition.y });
				m_Canvas->DrawText("2", { cursorPosition.x - 0.15f + 0.030f, cursorPosition.y - 0.015f }, 0.75f);
			}
			else
				m_Canvas->DrawText("HN H", { cursorPosition.x - 0.15f, cursorPosition.y });				

			if (m_PHLevel <= 2.3)
				m_Canvas->DrawText("OH", { -0.03f + cursorPosition.x + (0.245f + 0.045f) * f_ReferencedBuffer.size(), cursorPosition.y + (f_ReferencedBuffer.size() % 2 == 0 ? 0.0f : -0.05f) });
			else
				m_Canvas->DrawText("O-", { -0.03f + cursorPosition.x + (0.245f + 0.045f) * f_ReferencedBuffer.size(), cursorPosition.y + (f_ReferencedBuffer.size() % 2 == 0 ? 0.0f : -0.05f) });
		}

		for (size_t i{ 0U }; i < f_ReferencedBuffer.size(); ++i)
		{
			const char amino{ f_ReferencedBuffer[i] };
			
			BIO_UNLIKELY
			if(amino == '-')
				continue;

			BIO_ASSERT(s_DrawLists.find(amino) != s_DrawLists.end());
			{
				const auto& drawLists{ s_DrawLists[amino] };

				for (const auto& drawList : drawLists)
					if(drawList.MeetsPH(m_PHLevel))
						drawList.DrawList.Draw(m_Canvas, flip ? Vector2{ cursorPosition.x, cursorPosition.y } : cursorPosition, flip);
			}

			flip = !flip;
			cursorPosition = { cursorPosition.x + 0.245f, cursorPosition.y };
			
			if (i < f_ReferencedBuffer.size() - 1)
			{
				m_Canvas->DrawText("N", { cursorPosition.x + 0.014f, cursorPosition.y + (flip ? -0.05f : 0.02f) });
				m_Canvas->DrawText("H", { cursorPosition.x + 0.014f, cursorPosition.y + (flip ? -0.09f : 0.06f) });
			}
			
			cursorPosition.x += 0.045f;
		}
	}

	m_Canvas->EndDraw();
	m_ShouldRedraw = false;
}