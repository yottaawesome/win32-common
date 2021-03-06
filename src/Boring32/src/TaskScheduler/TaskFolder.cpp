#include "pch.hpp"
#include "include/Error/Error.hpp"
#include "include/TaskScheduler/TaskFolder.hpp"

namespace Boring32::TaskScheduler
{
	using Microsoft::WRL::ComPtr;

	TaskFolder::~TaskFolder()
	{
		Close();
	}

	TaskFolder::TaskFolder(ComPtr<ITaskFolder> taskFolder)
	:	m_taskFolder(std::move(taskFolder))
	{ }

	void TaskFolder::Close() noexcept
	{
		if (m_taskFolder)
			m_taskFolder = nullptr;
	}

	std::vector<RegisteredTask> TaskFolder::GetTasks()
	{
		if (m_taskFolder == nullptr)
			throw std::runtime_error(__FUNCSIG__ ": m_taskFolder is nullptr");

		ComPtr<IRegisteredTaskCollection> collection;
		HRESULT hr = m_taskFolder->GetTasks(0, &collection);
		if (FAILED(hr))
			throw Error::ComError(__FUNCSIG__ ": failed to acquire tasks", hr);

		LONG count = 0;
		collection->get_Count(&count);
		if (FAILED(hr))
			throw Error::ComError(__FUNCSIG__ ": failed to acquire task count", hr);

		if (count == 0)
			return {};

		std::vector<RegisteredTask> results;
		// Collection counts are 1-indexed
		for (LONG i = 1; i <= count; i++)
		{
			ComPtr<IRegisteredTask> task;
			hr = collection->get_Item(_variant_t(i), &task);
			if (FAILED(hr))
				throw Error::ComError(__FUNCSIG__ ": failed to get task item", hr);

			results.push_back(task);
		}

		return results;
	}

	std::optional<RegisteredTask> TaskFolder::GetTask(const std::wstring& name)
	{
		std::vector<RegisteredTask> tasks = GetTasks();
		for (const auto& task : tasks)
			if (task.GetName() == name)
				return task;
		return std::nullopt;
	}

	void TaskFolder::SaveOrUpdate(
		const RegisteredTask& task,
		const TASK_LOGON_TYPE logonType
	)
	{
		if (m_taskFolder == nullptr)
			throw std::runtime_error(__FUNCSIG__ ": m_taskFolder is null");

		ComPtr<IRegisteredTask> registeredTask;
		
		HRESULT hr = m_taskFolder->RegisterTaskDefinition(
			_bstr_t(task.GetName().c_str()),
			task.GetTaskDefinition().Get(),
			TASK_CREATE_OR_UPDATE,
			_variant_t(),
			_variant_t(),
			logonType,
			_variant_t(L""),
			&registeredTask
		);
		if (FAILED(hr))
			throw Error::ComError(__FUNCSIG__ ": failed to save or update task", hr);
	}
}